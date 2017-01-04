/************************************************************************/
/*																		*/
/*	xillinio.h															*/
/*																		*/
/************************************************************************/
/*	Author: Jonathan Aubert												*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*		Class used to create a mapping between physical					*/
/*   	and virtual address spaces. Static function are intended		*/
/*      to read or write to physical address "from everywhere".			*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		16.12.2016(Jonathan Aubert) : First version						*/
/*																		*/
/************************************************************************/

#ifndef XILLINIO_H_
#define XILLINIO_H_

#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <ctype.h>
#include <sys/mman.h>
#include <map>
#include <utility>

class xil_lin_io {
public:

	///Contains the mapping between "key = physAddr ; value <virtAddr, size>"
	static std::map<u_int32_t, std::pair<u_int32_t, u_int32_t> > pToV;

	/**
	 * @brief Read the value located at physBaseAddr+offs
	 * @param physBaseAddr base address of the peripheral
	 * @param offs offset added to base address
	 * return the value read at the physical address
	 */
	static u_int32_t Xil_In32(u_int32_t physBaseAddr, u_int32_t offs);

	/**
	 * @brief Write the value located at physBaseAddr+offs
	 * @param physBaseAddr base address of the peripheral
	 * @param offs offset added to base address
	 * return the value read at the physical address
	 */
	static void Xil_Out32(u_int32_t physBaseAddr, u_int32_t offs, u_int32_t value);

	/**
	 * @brief Add to mapping of the class and mmap() a physical address range
	 * to the virtual address space
	 * @param physBaseAddr base address of the physical address range
	 * @param size of the address range to map in byte
	 * return 0 on success
	 */
	int32_t mapAddAddr(u_int32_t physBaseAddr, size_t size);

	/**
	 * @brief Get the virtual base Address for the physical base address
	 * @param physBaseAddr base address of the peripheral
	 * return the virtual base Address
	 */
	u_int32_t getVirtAddr(u_int32_t physBaseAddr) const;

	/**
	 * @brief Print the current Mapping
	 */
	void printMap() const;

	/**
	 * @brief Constructor
	 */
	xil_lin_io();
	/**
	 * @brief Destructor, close fd and munmap()
	 */
	virtual ~xil_lin_io();
private:
	static int fd;	///File descriptor used on "/dev/mem"
};

#endif /* XILLINIO_H_ */
