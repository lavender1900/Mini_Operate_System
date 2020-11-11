#include	"const.h"
#include	"type.h"
#include	"protect.h"

PUBLIC void* kmemcpy(void* src, void* dest, int size);
PUBLIC void* disp_str(void* src);

PUBLIC u8		gdt_ptr[6];
PUBLIC DESCRIPTOR	gdt[GDT_SIZE];

PUBLIC void cstart() {
 disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n-----------\"cstart\" begins-----------\n");

 kmemcpy((void*) (*((u32*) (&gdt_ptr[2]))), &gdt,
	*((u16*) (&gdt_ptr[0])) + 1
 );
 
 u16* p_gdt_limit = (u16*) (&gdt_ptr[0]);
 u32* p_gdt_base = (u32*) (&gdt_ptr[2]);
 *p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
 *p_gdt_base = (u32) &gdt; 
}
