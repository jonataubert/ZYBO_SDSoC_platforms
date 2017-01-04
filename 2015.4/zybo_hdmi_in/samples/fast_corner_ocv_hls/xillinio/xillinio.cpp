/************************************************************************/
/*																		*/
/*	xillinio.cpp														*/
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

#include <xillinio/xillinio.h>

std::map<u_int32_t, std::pair<u_int32_t, size_t> > xil_lin_io::pToV;
int xil_lin_io::fd = -1;

xil_lin_io::xil_lin_io()
{
}

u_int32_t xil_lin_io::Xil_In32(u_int32_t physBaseAddr, u_int32_t offs)
{
	return *(u_int32_t*)(pToV[physBaseAddr].first+offs);
}

void xil_lin_io::Xil_Out32(u_int32_t physBaseAddr, u_int32_t offs, u_int32_t value)
{
	*(u_int32_t*)(pToV[physBaseAddr].first+offs) = value;
}

int32_t xil_lin_io::mapAddAddr(u_int32_t physBaseAddr, size_t size){
	if (fd < 1)	//when uninitialized
	{
		// Open dev/mem character device
		fd = open("/dev/mem", O_RDWR);
		if (fd < 1) {
			printf("Couldn't open /dev/mem\n");
			return EXIT_FAILURE;
		}
	}

	//if mapping exists
	if (pToV.find(physBaseAddr) != pToV.end())
	{
		printf("Already mapped\n");
		return EXIT_FAILURE;
	}

	//perform mmap
	u_int32_t *virt_addr = (u_int32_t*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)physBaseAddr);

	if(virt_addr == MAP_FAILED)
	{
		printf("VirtualAddress mapping for absolute memory access failed.\n");
		return EXIT_FAILURE;
	}
	//Add to the map shared obj
	pToV[physBaseAddr] = std::make_pair((u_int32_t)virt_addr,size);

	return EXIT_SUCCESS;
}


u_int32_t xil_lin_io::getVirtAddr(u_int32_t physBaseAddr) const
{
	return pToV[physBaseAddr].first;
}

void xil_lin_io::printMap() const
{
	for (std::map<u_int32_t, std::pair<u_int32_t, u_int32_t> >::iterator it=this->pToV.begin(); it!=this->pToV.end(); ++it)
	    printf("physAddr : 0x%08x virtAddr : 0x%08x sizeByte : %u\n",it->first,it->second.first, it->second.second);
}

xil_lin_io::~xil_lin_io()
{
	close(this->fd);
	//unmap
	for (std::map<u_int32_t, std::pair<u_int32_t, u_int32_t> >::iterator it=this->pToV.begin(); it!=this->pToV.end(); ++it)
	{
		munmap((void *)it->second.first, it->second.second);
	}
}

