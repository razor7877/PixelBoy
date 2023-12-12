#ifndef FRONTEND_HPP
#define FRONTEND_HPP

struct Frontend
{
	int (*start)(); // Returns 0 if no errors during start
	int (*update)(); // Returns 1 to exit emulation loop, continues otherwise
	void (*stop)();
	void (*update_io)();
};

#endif