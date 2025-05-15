This is a repository for implementations of data structures and some known functions in C or CPP. The programms for the languages are in the respective folders.

There is a implementation of a heap allocator in C using the mmap syscal from linux. It uses a double linked list to fragment the available memory and keep track of the memory allocated by the user. The biggest size for allocation is 256 bytes.
It also has a free function for releasing the memory allocated and the function freeallm for freeing all of the memory allocated with mmap.

There is also a implementation of a dynamic vector using malloc that works for similarly to the vector from cpp stl with generic types.

There is a implementation of a dynamic vector, heap data structure and segment tree in cpp. The heap and segment tree use the vector library implemented.
