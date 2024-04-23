#include <string.h>
#include <stdbool.h>

#include "ppu.h"
#include "cpu.h"
#include "interrupts.h"
#include "memory.h"

#include "logging.h"

uint32_t ppu_cycle_count = 0;

uint8_t vram[2][8192] = {0};
uint8_t OAM[160] = {0};
uint8_t LCDC = 0x91;
uint8_t STAT = 0x81;
uint8_t SCY = 0;
uint8_t SCX = 0;
uint8_t LY = 0x91;
uint8_t LYC = 0x00;
uint8_t DMA = 0xFF;
uint8_t BGP = 0xFC;
uint8_t OBP0 = 0;
uint8_t OBP1 = 0;
uint8_t WY = 0;
uint8_t WX = 0;

// CGB only
uint8_t bg_palette_RAM[64] = {0};
uint8_t obj_palette_RAM[64] = {0};

uint8_t VBK = 0xFE;
uint8_t HDMA1 = 0xFF;
uint8_t HDMA2 = 0xFF;
uint8_t HDMA3 = 0xFF;
uint8_t HDMA4 = 0xFF;
uint8_t HDMA5 = 0xFF;
uint8_t BGPI = 0;
uint8_t BGPD = 0;
uint8_t OBPI = 0;
uint8_t OBPD = 0;
uint8_t OPRI = 0x01;

uint32_t dot_counter = 0;
uint32_t dot_start = 0;
uint32_t dot_end = 0;
bool start_frame = false;
bool new_frame_ready = false;

bool running_hdma = false;

// GL_RGB5 : 2 bytes per pixel
uint8_t frame_buffer[160 * 144 * 3] = {0};

void tick_ppu(uint8_t cycles)
{
	if (LCDC & LCD_ENABLE)
	{
		ppu_cycle_count += cycles;

		if (ppu_cycle_count > PPU_FREQ)
		{
			ppu_cycle_count %= PPU_FREQ;
			//printf("Ran full PPU cycle\n");
		}
			

		if (LY == LYC)
		{
			STAT |= LYC_COMPARE_FLAG;
			if (STAT & LYC_INTERRUPT_ENABLED) { interrupt_request(INTERRUPT_LCD_STAT); }
		}
		else { STAT &= ~LYC_COMPARE_FLAG; }

		step_ppu(cycles);
	}
}

void step_ppu(uint8_t cycles)
{
	for (uint8_t i = 0; i < cycles; i += 4)
	{
		if (start_frame)
		{
			//printf("New frame at cpu cycle %d\n", cycle_count);
			dot_start = cycle_count;
			dot_end = cycle_count;
			start_frame = false;
		}

		dot_counter += 4;
		dot_end += 4;

		if (LY < 144)
		{
			if (dot_counter < 80) // OAM Scan
			{
				if (ppu_mode() != LCD_MODE_2)
				{
					STAT = (STAT & 0xFC) | LCD_MODE_2;
					if (STAT & OAM_INTERRUPT_ENABLED) { interrupt_request(INTERRUPT_LCD_STAT); }
				}
			}
			else if (dot_counter < 252) // Drawing
			{
				if (ppu_mode() != LCD_MODE_3)
				{
					STAT = (STAT & 0xFC) | LCD_MODE_3;
					if (run_as_cgb)
						draw_gbc_scanline();
					else
						draw_gb_scanline();
				}
						
			}
			else if (dot_counter < 456) // HBLANK
			{
				if (ppu_mode() != LCD_MODE_0)
				{
					STAT = (STAT & 0xFC) | LCD_MODE_0;
					if (STAT & HBLANK_INTERRUPT_ENABLED) { interrupt_request(INTERRUPT_LCD_STAT); }

					if (running_hdma) // If active HBlank DMA
					{
						uint8_t remaining_length = HDMA5 & 0b01111111;

						if (remaining_length > 0)
						{
							uint16_t source_address = (HDMA1 << 8) | (HDMA2 & 0xF0); // 4 lower bits are ignored
							uint16_t destination_address = ((HDMA3 << 8) & 0x1F) | (HDMA4 & 0xF0); // 3 upper and 4 lower bits are ignored
							destination_address += 0x8000; // Offset to map into VRAM
							remaining_length--;
							log_debug("HBLANK DMA: Source ADR %x Dest ADR %x Transfer remaining %x LY %x\n",
								source_address, destination_address, remaining_length, LY);

							// Write 16 bytes to VRAM
							for (int i = 0; i < 16; i++)
							{
								write_byte(destination_address + i, read_byte(source_address + i));
							}

							// Update source and destination with new offset
							uint16_t new_source = source_address + 0x10;
							uint16_t new_destination = destination_address + 0x10;

							log_debug("New source %x new dest %x\n", new_source, new_destination);

							HDMA1 = new_source >> 8;
							HDMA2 = new_source & 0xFF;
							HDMA3 = new_destination >> 8;
							HDMA4 = new_destination & 0xFF;

							if (remaining_length == 0) // Transfer finished
							{
								HDMA5 = 0xFF;
								running_hdma = false;
							}
							else
								HDMA5 = 0x80 | remaining_length;

							log_debug("New HDMA5 %x\n", HDMA5);
						}
					}
				}
			}
			else
			{
				dot_counter = 0;
				LY++;
			}
		}
		else if (LY < 154) // VBLANK
		{
			if (ppu_mode() != LCD_MODE_1)
			{
				STAT = (STAT & 0xFC) | LCD_MODE_1;
				interrupt_request(INTERRUPT_VBLANK);
				if (STAT & VBLANK_INTERRUPT_ENABLED) { interrupt_request(INTERRUPT_LCD_STAT); }
			}

			if (dot_counter >= 456)
			{
				dot_counter %= 456;
				LY++;
			}
		}
		else // Frame finished drawing, reset scanline counter
		{
			LY = 0;
			dot_counter %= 456;
			//printf("Finished drawing frame START %d END %d\n", dot_start, dot_end);
			start_frame = true;
			// Tells the CPU that a new frame is ready for execute_cycle() method
			new_frame_ready = true;
		}
	}
}

void draw_gb_scanline()
{
	draw_gb_tiles();
	draw_gb_sprites();
}

void draw_gb_tiles()
{
	uint16_t tile_data = 0;
	uint16_t background_memory = 0;
	bool unsig = true;

	uint8_t win_x = WX - 7;

	bool using_window = false;

	if (LCDC & WINDOW_ENABLE) // Draw window
	{
		if (WY <= LY) // If current scanline is within window Y pos
			using_window = true;
	}

	if (LCDC & TILE_DATA_AREA)
	{
		tile_data = 0x8000;
	}
	else
	{
		tile_data = 0x8800;
		unsig = false;
	}

	if (using_window)
	{
		if (LCDC & WINDOW_DATA_AREA)
			background_memory = 0x9C00;
		else
			background_memory = 0x9800;
	}
	else
	{
		if (LCDC & BG_DATA_AREA)
			background_memory = 0x9C00;
		else
			background_memory = 0x9800;
	}

	uint8_t y_pos = 0;

	// y_pos is used to calculate which of 32 vertical tiles the current
	// scanline is drawing
	if (!using_window)
		y_pos = SCY + LY;
	else
		y_pos = LY - WY;

	// Which of the 8 vertical pixels of the current tile is the scanline on?
	uint16_t tile_row = (y_pos / 8) * 32;

	// Draw the 160 horizontal pixels for this scanline
	for (uint8_t pixel = 0; pixel < 160; pixel++)
	{
		uint8_t x_pos = pixel + SCX;

		// Translate current x position to window space if necessary
		if (using_window)
		{
			if (pixel >= win_x)
				x_pos = pixel - win_x;
		}

		// Which of the 32 horizontal lines does this x pos fall within?
		uint16_t tile_col = x_pos / 8;
		int16_t tile_num;

		// Get the tile identity number (can be signed/unsigned)
		uint16_t tile_address = background_memory + tile_row + tile_col;
		if (unsig)
			tile_num = (uint8_t)vram[0][tile_address - 0x8000];
		else
			tile_num = (int8_t)vram[0][tile_address - 0x8000];

		// Deduce where this tile identifier is in memory
		uint16_t tile_location = tile_data;
		if (unsig)
			tile_location += (tile_num * 16);
		else
			tile_location += ((tile_num + 128) * 16);

		// Find correct vertical line we're on of the tile to get tile data in memory
		uint8_t line = y_pos % 8;
		line *= 2; // Each line takes up 2 bytes of memory
		uint8_t data_1 = vram[0][tile_location + line - 0x8000];
		uint8_t data_2 = vram[0][tile_location + line + 1 - 0x8000];

		// pixel 0 : bit 7 of data_1 and data_2
		// pixel 1 : bit 6 etc.
		int color_bit = x_pos % 8;
		color_bit -= 7;
		color_bit *= -1;

		// Combine the two to get colour id for this pixel in the tile
		uint8_t color_mask = 1 << color_bit;
		uint8_t first_bit = (data_1 & color_mask) >> color_bit;
		uint8_t second_bit = (data_2 & color_mask) >> color_bit;
		uint8_t color_num = first_bit | (second_bit << 1);

		uint8_t color = 0;

		// Get actual color from palette as 2 bit value
		switch (color_num)
		{
			case 0x0:
				color = BGP & 0x03;
				break;

			case 0x1:
				color = (BGP & 0x0C) >> 2;
				break;

			case 0x2:
				color = (BGP & 0x30) >> 4;
				break;

			case 0x3:
				color = (BGP & 0xC0) >> 6;
				break;
		}
		
		uint8_t buffer_color = 0;

		// Get corresponding RGBA color from the 2 bit value
		switch (color)
		{
			case 0x0: // White
				buffer_color = 0xFF;
				break;

			case 0x1: // Light gray
				buffer_color = 0xAA;
				break;

			case 0x2: // Dark gray
				buffer_color = 0x55;
				break;

			case 0x3: // Black
				buffer_color = 0x00;
				break;
		}

		frame_buffer[LY * 160 * 3 + pixel * 3] = buffer_color;
		frame_buffer[LY * 160 * 3 + pixel * 3 + 1] = buffer_color;
		frame_buffer[LY * 160 * 3 + pixel * 3 + 2] = buffer_color;
	}
}

void draw_gb_sprites()
{
	bool obj_large = false;

	if (LCDC & OBJ_SIZE)
		obj_large = true;

	for (uint8_t sprite = 0; sprite < 40; sprite++)
	{
		uint8_t index = sprite * 4; // Sprite occupies 4 bytes in OAM
		uint8_t y_pos = OAM[index] - 16;
		uint8_t x_pos = OAM[index + 1] - 8;
		uint8_t tile_index = OAM[index + 2];
		uint8_t attributes = OAM[index + 3];

		bool x_flip = (attributes & 0x20) >> 4; // 5th bit
		bool y_flip = (attributes & 0x40) >> 5; // 6th bit

		uint8_t y_size = 8;
		if (obj_large)
			y_size = 16;

		if ((LY >= y_pos) && (LY < (y_pos + y_size)))
		{
			int line = LY - y_pos;

			// Read sprite backwards in Y axis
			if (y_flip)
			{
				line -= y_size;
				line *= -1;
			}

			line *= 2;
			uint16_t data_address = tile_index * 16 + line;
			uint8_t data_1 = vram[0][data_address];
			uint8_t data_2 = vram[0][data_address + 1];

			for (int tile_pixel = 7; tile_pixel >= 0; tile_pixel--)
			{
				uint8_t color_bit = tile_pixel;
				// Read sprite backwards in X axis
				if (x_flip)
				{
					color_bit -= 7;
					color_bit *= -1;
				}

				// Combine the two to get colour id for this pixel in the tile
				uint8_t color_mask = 1 << color_bit;
				uint8_t first_bit = (data_1 & color_mask) >> color_bit;
				uint8_t second_bit = (data_2 & color_mask) >> color_bit;
				uint8_t color_num = first_bit | (second_bit << 1);

				uint8_t color = 0;

				// Get actual color from palette as 2 bit value
				switch (color_num)
				{
					case 0x0:
						color = BGP & 0x03;
						break;

					case 0x1:
						color = (BGP & 0x0C) >> 2;
						break;

					case 0x2:
						color = (BGP & 0x30) >> 4;
						break;

					case 0x3:
						color = (BGP & 0xC0) >> 6;
						break;
				}

				uint8_t buffer_color = 0;

				// Get corresponding RGBA color from the 2 bit value
				switch (color)
				{
					case 0x0: // White
						buffer_color = 0xFF;
						break;

					case 0x1: // Light gray
						buffer_color = 0xAA;
						break;

					case 0x2: // Dark gray
						buffer_color = 0x55;
						break;

					case 0x3: // Black
						buffer_color = 0x00;
						break;
				}

				int x_pix = 0 - tile_pixel;
				x_pix += 7;

				int pixel = x_pos + x_pix;

				//printf("Draw pixel at x %d y %d\n", LY, pixel);
				// White pixel for sprites is transparent
				if (buffer_color != 0xFF)
				{
					frame_buffer[LY * 160 * 3 + pixel * 3] = buffer_color;
					frame_buffer[LY * 160 * 3 + pixel * 3 + 1] = buffer_color;
					frame_buffer[LY * 160 * 3 + pixel * 3 + 2] = buffer_color;
				}
			}
		}
	}
}

void draw_gbc_scanline()
{
	draw_gbc_tiles();
	draw_gbc_sprites();
}

void draw_gbc_tiles()
{
	uint16_t tile_data = 0;
	uint16_t background_memory = 0;
	bool unsig = true;

	uint8_t win_x = WX - 7;

	bool using_window = false;

	if (LCDC & WINDOW_ENABLE) // Draw window
	{
		if (WY <= LY) // If current scanline is within window Y pos
			using_window = true;
	}

	if (LCDC & TILE_DATA_AREA)
	{
		tile_data = 0x8000;
	}
	else
	{
		tile_data = 0x8800;
		unsig = false;
	}

	if (using_window)
	{
		if (LCDC & WINDOW_DATA_AREA)
			background_memory = 0x9C00;
		else
			background_memory = 0x9800;
	}
	else
	{
		if (LCDC & BG_DATA_AREA)
			background_memory = 0x9C00;
		else
			background_memory = 0x9800;
	}

	uint8_t y_pos = 0;

	// y_pos is used to calculate which of 32 vertical tiles the current
	// scanline is drawing
	if (!using_window)
		y_pos = SCY + LY;
	else
		y_pos = LY - WY;

	// Which of the 8 vertical pixels of the current tile is the scanline on?
	uint16_t tile_row = (y_pos / 8) * 32;

	// Draw the 160 horizontal pixels for this scanline
	for (uint8_t pixel = 0; pixel < 160; pixel++)
	{
		uint8_t x_pos = pixel + SCX;

		// Translate current x position to window space if necessary
		if (using_window)
		{
			if (pixel >= win_x)
				x_pos = pixel - win_x;
		}

		// Which of the 32 horizontal lines does this x pos fall within?
		uint16_t tile_col = x_pos / 8;
		int16_t tile_num;
		int16_t tile_attributes;

		// Get the tile identity number (can be signed/unsigned)
		uint16_t tile_address = background_memory + tile_row + tile_col;
		if (unsig)
		{
			tile_num = (uint8_t)vram[0][tile_address - 0x8000]; // Get tile index in bank 0
			tile_attributes = (uint8_t)vram[1][tile_address - 0x8000]; // Get tile attributes in bank 1
		}
		else
		{
			tile_num = (int8_t)vram[0][tile_address - 0x8000];
			tile_attributes = (int8_t)vram[1][tile_address - 0x8000];
		}

		bool x_flip = (tile_attributes & 0x20) >> 5; // x_flip is on bit 5
		bool y_flip = (tile_attributes & 0x40) >> 6; // y_flip is on bit 6

		// Deduce where this tile identifier is in memory
		uint16_t tile_location = tile_data;
		if (unsig)
			tile_location += (tile_num * 16);
		else
			tile_location += ((tile_num + 128) * 16);

		// Find correct vertical line we're on of the tile to get tile data in memory
		int line = y_pos % 8;
		if (y_flip)
		{
			line -= 7;
			line *= -1;
		}

		line *= 2; // Each line takes up 2 bytes of memory

		// Get data from VRAM
		uint8_t vram_bank = (tile_attributes & 0b1000) >> 3;
		uint8_t data_1 = vram[vram_bank][tile_location + line - 0x8000];
		uint8_t data_2 = vram[vram_bank][tile_location + line + 1 - 0x8000];

		// pixel 0 : bit 7 of data_1 and data_2
		// pixel 1 : bit 6 etc.

		int color_bit = x_pos % 8;

		// If no x flipping, we need to reverse how we read (pixel 0 maps to bit 7)
		if (!x_flip)
		{
			color_bit -= 7;
			color_bit *= -1;
		}

		// Combine the two to get colour id for this pixel in the tile
		uint8_t color_mask = 1 << color_bit;
		uint8_t first_bit = (data_1 & color_mask) >> color_bit;
		uint8_t second_bit = (data_2 & color_mask) >> color_bit;
		uint8_t color_num = first_bit | (second_bit << 1);

		uint16_t color = 0;
		uint8_t color_red = 0;
		uint8_t color_green = 0;
		uint8_t color_blue = 0;

		uint8_t palette = tile_attributes & 0b111;
		uint8_t palette_start = palette * 8; // Each palette is 8 bytes

		// Get actual color from palette RAM as 2 byte value
		switch (color_num)
		{
		case 0x0:
			color = bg_palette_RAM[palette_start] | (bg_palette_RAM[palette_start + 1] << 8);
			break;

		case 0x1:
			color = bg_palette_RAM[palette_start + 2] | (bg_palette_RAM[palette_start + 3] << 8);
			break;

		case 0x2:
			color = bg_palette_RAM[palette_start + 4] | (bg_palette_RAM[palette_start + 5] << 8);
			break;

		case 0x3:
			color = bg_palette_RAM[palette_start + 6] | (bg_palette_RAM[palette_start + 7] << 8);
			break;
		}

		color_red = (color & 0b11111);
		color_green = (color & 0b1111100000) >> 5;
		color_blue = (color & 0b111110000000000) >> 10;

		frame_buffer[LY * 160 * 3 + pixel * 3] = color_red << 3;
		frame_buffer[LY * 160 * 3 + pixel * 3 + 1] = color_green << 3;
		frame_buffer[LY * 160 * 3 + pixel * 3 + 2] = color_blue << 3; 
	}
}

void draw_gbc_sprites()
{
	bool obj_large = false;

	if (LCDC & OBJ_SIZE)
		obj_large = true;

	for (uint8_t sprite = 0; sprite < 40; sprite++)
	{
		uint8_t index = sprite * 4; // Sprite occupies 4 bytes in OAM
		uint8_t y_pos = OAM[index] - 16;
		uint8_t x_pos = OAM[index + 1] - 8;
		uint8_t tile_index = OAM[index + 2];
		uint8_t attributes = OAM[index + 3];

		bool x_flip = (attributes & 0x20) >> 4; // 5th bit
		bool y_flip = (attributes & 0x40) >> 5; // 6th bit

		uint8_t y_size = 8;
		if (obj_large)
			y_size = 16;

		if ((LY >= y_pos) && (LY < (y_pos + y_size)))
		{
			int line = LY - y_pos;

			// Read sprite backwards in Y axis
			if (y_flip)
			{
				line -= y_size;
				line *= -1;
			}

			line *= 2;
			uint16_t data_address = tile_index * 16 + line;
			uint8_t data_1 = vram[(attributes & 0b1000) >> 3][data_address];
			uint8_t data_2 = vram[(attributes & 0b1000) >> 3][data_address + 1];

			for (int tile_pixel = 7; tile_pixel >= 0; tile_pixel--)
			{
				uint8_t color_bit = tile_pixel;
				// Read sprite backwards in X axis
				if (x_flip)
				{
					color_bit -= 7;
					color_bit *= -1;
				}

				// Combine the two to get colour id for this pixel in the tile
				uint8_t color_mask = 1 << color_bit;
				uint8_t first_bit = (data_1 & color_mask) >> color_bit;
				uint8_t second_bit = (data_2 & color_mask) >> color_bit;
				uint8_t color_num = first_bit | (second_bit << 1);

				uint16_t color = 0;
				uint8_t color_red = 0;
				uint8_t color_green = 0;
				uint8_t color_blue = 0;

				uint8_t palette = attributes & 0b111;
				uint8_t palette_start = palette * 8; // Each palette is 8 bytes

				// Get actual color from palette RAM as 2 byte value
				switch (color_num)
				{
				case 0x1:
					color = obj_palette_RAM[palette_start + 2] | (obj_palette_RAM[palette_start + 3] << 8);
					break;

				case 0x2:
					color = obj_palette_RAM[palette_start + 4] | (obj_palette_RAM[palette_start + 5] << 8);
					break;

				case 0x3:
					color = obj_palette_RAM[palette_start + 6] | (obj_palette_RAM[palette_start + 7] << 8);
					break;
				}

				color_red = (color & 0b11111);
				color_green = (color & 0b1111100000) >> 5;
				color_blue = (color & 0b111110000000000) >> 10;

				int x_pix = 0 - tile_pixel;
				x_pix += 7;

				int pixel = x_pos + x_pix;

				//printf("Draw pixel at x %d y %d\n", LY, pixel);
				// White pixel for sprites is transparent
				// TODO: Add transparency
				if (color_num != 0)
				{
					frame_buffer[LY * 160 * 3 + pixel * 3] = color_red << 3;
					frame_buffer[LY * 160 * 3 + pixel * 3 + 1] = color_green << 3;
					frame_buffer[LY * 160 * 3 + pixel * 3 + 2] = color_blue << 3;
				}
			}
		}
	}
}

void reset_ppu()
{
	ppu_cycle_count = 0;

	memset(&vram[0], 0, sizeof(vram[0]));
	memset(&vram[1], 0, sizeof(vram[1]));
	memset(&OAM, 0, sizeof(OAM));
	LCDC = 0x91;
	STAT = 0x81;
	SCY = 0;
	SCX = 0;
	LY = 0x91;
	LYC = 0x00;
	DMA = 0xFF;
	BGP = 0xFC;
	OBP0 = 0;
	OBP1 = 0;
	WY = 0;
	WX = 0;

	memset(&bg_palette_RAM, 0, sizeof(bg_palette_RAM));
	memset(&obj_palette_RAM, 0, sizeof(obj_palette_RAM));
	VBK = 0xFE;
	HDMA1 = 0xFF;
	HDMA2 = 0xFF;
	HDMA3 = 0xFF;
	HDMA4 = 0xFF;
	HDMA5 = 0xFF;
	BGPI = 0;
	BGPD = 0;
	OBPI = 0;
	OBPD = 0;
	OPRI = 0x01;

	dot_counter = 0;
	dot_start = 0;
	dot_end = 0;
	start_frame = false;
	new_frame_ready = false;
}

uint8_t ppu_mode()
{
	return STAT & 0x03;
}

uint8_t read_vram(uint16_t address)
{
	if ((STAT & 0x03) == 0x03) // VRAM inaccessible during mode 3
		return 0xFF;

	if (run_as_cgb && (VBK & 0b1)) // In CGB mode, we need to handle bank 1 reads
		return vram[1][address];
	else
		return vram[0][address];
}

void write_vram(uint16_t address, uint8_t value)
{
	if ((STAT & 0x03) == 0x03) // VRAM inaccessible during mode 3, ignore writes
	{
		log_warning("Ignored VRAM write during mode 3!\n");
		return;
	}
	if (run_as_cgb && (VBK & 0b1)) // In CGB mode, we need to handle bank 1 writes
		vram[1][address] = value;
	else
		vram[0][address] = value;
}

uint8_t read_oam(uint16_t address)
{
	// OAM inaccessible during mode 2 and 3 and DMA transfer
	if (((STAT & 0x03) == 0x03) || ((STAT & 0x03) == 0x02))
		return 0xFF;
	else
		return OAM[address];
}

void write_oam(uint16_t address, uint8_t value)
{
	// OAM inaccessible during mode 2 and 3
	if (((STAT & 0x03) == 0x03) || ((STAT & 0x03) == 0x02))
		return;
	else
		OAM[address] = value;
}

uint8_t read_ppu(uint16_t address)
{
	//log_warning("PPU reg read ADR %x\n", address);

	if (address == 0xFF40)
		return LCDC;

	if (address == 0xFF41)
		return STAT;

	if (address == 0xFF42)
		return SCY;

	if (address == 0xFF43)
		return SCY;

	if (address == 0xFF44)
		return LY;

	if (address == 0xFF45)
		return LYC;

	if (address == 0xFF46)
		return DMA;

	if (address == 0xFF47)
		return BGP;

	if (address == 0xFF48)
		return OBP0;

	if (address == 0xFF49)
		return OBP1;

	if (address == 0xFF4A)
		return WY;

	if (address == 0xFF4B)
		return WX;

	if (address == 0xFF4F)
		return VBK;

	if (address == 0xFF51)
		return HDMA1;

	if (address == 0xFF52)
		return HDMA2;

	if (address == 0xFF53)
		return HDMA3;

	if (address == 0xFF54)
		return HDMA4;

	if (address == 0xFF55)
		return HDMA5;

	if (address == 0xFF68)
		return BGPI;

	if (address == 0xFF69)
	{
		return bg_palette_RAM[BGPI & 0x3F];
	}

	if (address == 0xFF6A)
		return OBPI;

	if (address == 0xFF6B)
		return bg_palette_RAM[OBPI & 0x3F];

	return 0;
}

void write_ppu(uint16_t address, uint8_t value)
{
	//log_error("PPU reg write ADR %x\n", address);
	if (address == 0xFF40)
		update_LCDC(value);

	if (address == 0xFF41) // Lower 2 bits are read only
		STAT = (value & 0xFC) | (STAT & 0x03);

	if (address == 0xFF42)
		SCY = value;

	if (address == 0xFF43)
		SCX = value;

	if (address == 0xFF44) // LY is read only
		return;

	if (address == 0xFF45)
		LYC = value;

	if (address == 0xFF46)
	{
		DMA = value;
		dma_transfer();
	}

	if (address == 0xFF47)
		BGP = value;

	if (address == 0xFF48)
		OBP0 = value;

	if (address == 0xFF49)
		OBP1 = value;

	if (address == 0xFF4A)
		WY = value;

	if (address == 0xFF4B)
		WX = value;

	if (address == 0xFF4F)
		VBK = value & 0b1;

	// TODO : Correct write behavior to these registers
	if (address == 0xFF51)
		HDMA1 = value;

	if (address == 0xFF52)
		HDMA2 = value;

	if (address == 0xFF53)
		HDMA3 = value;

	if (address == 0xFF54)
		HDMA4 = value;

	if (address == 0xFF55) // HDMA5 writes trigger VRAM DMA
	{
		log_debug("HDMA5 Write value %x\n", value);

		bool active_dma = (HDMA5 & 0x80) >> 7;
		bool is_set = (value & 0x80) >> 7;

		if (!is_set && !running_hdma) // General purpose DMA
		{
			log_debug("VRAM DMA Triggered with mode 0\n");

			uint8_t transfer_length = (value & 0b01111111); // Get length from 7 lower bits
			transfer_length++; // Increment by 1
			transfer_length *= 0x10; // Multiply by 16

			uint16_t source_address = (HDMA1 << 8) | (HDMA2 & 0xF0); // 4 lower bits are ignored
			uint16_t destination_address = ((HDMA3 << 8) & 0x1F) | (HDMA4 & 0xF0); // 3 upper and 4 lower bits are ignored
			destination_address += 0x8000; // Offset to map into VRAM

			log_debug("Source ADR %x Dest ADR %x Transfer length %x\n", source_address, destination_address, transfer_length);

			for (int i = 0; i < transfer_length; i++)
			{
				write_byte(destination_address + i, read_byte(source_address + i));
			}

			HDMA5 = 0xFF; // Transfer is done
		}
		else if (!is_set && running_hdma) // Setting bit 7 to 0 during HBlank DMA stops it
		{
			log_debug("Turn OFF running HDMA\n");
			running_hdma = false;
		}
		else if (is_set) // Start HBlank DMA
		{
			running_hdma = true;
			log_info("Starting HDMA!\n");
		}

		HDMA5 = value & 0x7F;
	}

	if (address == 0xFF68)
		BGPI = value;

	if (address == 0xFF69) // Background palette data write
	{
		// TODO : Disable writes during mode 3
		bg_palette_RAM[BGPI & 0x3F] = value;
		if (BGPI & 0x80) // If auto-increment on
			BGPI++;
		//log_warning("Background palette RAM write ADR %x VALUE %x pc %x\n", BGPI & 0x3F, value, pc);
	}

	if (address == 0xFF6A)
		OBPI = value;

	if (address == 0xFF6B) // OBJ palette data write
	{
		obj_palette_RAM[OBPI & 0x3F] = value;
		if (OBPI & 0x80) // If auto-increment on
			OBPI++;
		//log_warning("OBJ palette RAM write ADR %x VALUE %x\n", OBPI & 0x3F, value);
	}
}

void update_LCDC(uint8_t value)
{
	if ((value & LCD_ENABLE) == 0 && (LCDC & LCD_ENABLE)) // If LCD is being turned off
	{
		if (ppu_mode() != LCD_MODE_0)
			STAT = (STAT & 0xFC) | LCD_MODE_0;

		LY = 0;
		dot_counter = 0;
		LCDC = value & 0xFC;
		
	}
	else { LCDC = value; }
}
