// Selector Local Descriptor Indicator
#define  SA_LOCAL			0x04

// Selector Rest Privilege Level
#define  SA_RPL0				0
#define  SA_RPL1				1
#define  SA_RPL2				2
#define  SA_RPL3				3

// Segment Attributes Composite
// Attribute Mapping
// | G | D/B | 0 | AVL | Segment Limit 2 | P | DPL | S | Type|

#define  DA_PRESENT			0x80 	// Segment is present

// Descriptor Privilege Level
#define  DA_DPL0				0  		// Descriptor Privilege is 0
#define  DA_DPL1				0x20		// Descriptor Privilege is 1
#define  DA_DPL2				0x40		// Descriptor Privilege is 2
#define  DA_DPL3				0x60		// Descritpor Privilege is 3

// Segment Category
#define  DA_CATEGORY_DC			0x10		// Segment is code segment or data segment
#define  DA_CATEGORY_SG			0		// Segment is system segment or gate segment

// Code Or Data Segment Type
#define  DA_TYPE_RO			0		// Read Only
#define  DA_TYPE_RO_ACCESSED		1		// Read Only And Accessed
#define  DA_TYPE_RW			2		// Read and Write
#define  DA_TYPE_RW_ACCESSED		3		// Read Write And Accessed
#define  DA_TYPE_RO_GTD			4		// Read Only, Grows towards down
#define  DA_TYPE_RO_GTD_ACCESSED	5		// Read Only, Grows towards down, Accessed
#define  DA_TYPE_RW_GTD			6		// Read Write, Grows towards down
#define  DA_TYPE_RW_GTD_ACCESSED	7		// Read Write, Grows towards down, Accessed
#define  DA_TYPE_EO			8		// Execute Only
#define  DA_TYPE_EO_ACCESSED		9		// Execute Only, Accessed
#define  DA_TYPE_ER			0x0A		// Execute, Read
#define  DA_TYPE_ER_ACCESSED		0x0B		// Execute, Read, Accessed
#define  DA_TYPE_EO_CF			0x0C		// Execute Only, Conform Segment
#define  DA_TYPE_EO_CF_ACCESSED		0x0D		// Execute Only, Conform Segment, Accessed
#define  DA_TYPE_ER_CF			0x0E		// Execute, Read, Conform Segment
#define  DA_TYPE_ER_CF_ACCESSED		0x0F		// Execute, Read, Conform Segment, Accessed

// System Or Gate Segment Type
#define  DA_TYPE_UNDEFINE0		0		// Undefined0
#define  DA_TYPE_286TSS			1		// Available 286 TSS
#define  DA_TYPE_LDT			2		// Local Descriptor Table
#define  DA_TYPE_286TSS_BUSY		3		// Busy 286 TSS
#define  DA_TYPE_286CALL_GATE		4		// 286 Call Gate
#define  DA_TYPE_TASK_GATE		5		// Tast Gate
#define  DA_TYPE_286INT_GATE		6		// 286 Interrupt Gate
#define  DA_TYPE_286TRAP_GATE		7		// 286 Trap Gate
#define  DA_TYPE_UNDEFINE1		8		// Undefined 1
#define  DA_TYPE_386TSS			9		// Available 386 TSS
#define  DA_TYPE_UNDEFINE2		0x0A		// Undefined 2
#define  DA_TYPE_386TSS_BUSY		0x0B		// Busy 386 TSS
#define  DA_TYPE_386CALL_GATE		0x0C		// 386 Call Gate
#define  DA_TYPE_UNDEFINE3		0x0D		// Undefined 3
#define  DA_TYPE_386INT_GATE		0x0E		// 386 Interrupt Gate
#define  DA_TYPE_386TRAP_GATE		0x0F		// 386 Trap Gate

// Segment Granularity
#define  DA_GRAN_BYTE			0		// Segment Granularity is byte
#define  DA_GRAN_4KB			0x8000		// Segment Granularity is 4KB

// Code Segment D/B
#define  DA_CODE_32BIT			0x4000		// Code Segment With Implicit 32 bit Operand
#define  DA_CODE_16BIT			0		// Code Segment With Implicit 16 bit Operand

// Data Segment Towards Down D/B
#define  DA_DATA_4G_UPPER_LIMIT		0x4000		// Data Segment With 4GB Upper Limit
#define  DA_DATA_64K_UPPER_LIMIT	0		// Data Segment With 64KB Upper Limit

// Stack Segment D/B
#define  DA_STACK_USE_ESP		0x4000		// Stack Segment, When using implicit instructions, use esp as stack pointer
#define  DA_STACK_USE_SP		0		// Stack Segment, When using implicit instructions, use sp as stack pointer

// Common Composite

#define  DA_LDT				DA_PRESENT | DA_CATEGORY_SG | DA_TYPE_LDT

// TSS
#define  DA_386TSS			DA_PRESENT | DA_CATEGORY_SG | DA_TYPE_386TSS

// Gates
#define  DA_386CALL_GATE		DA_PRESENT | DA_CATEGORY_SG | DA_TYPE_386CALL_GATE
#define  DA_386INT_GATE			DA_PRESENT | DA_CATEGORY_SG | DA_TYPE_386INT_GATE

// Different Code Segments
#define  DA_32BIT_EXE_ONLY_CODE			DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_EO // 32bit execute only code segment
#define  DA_32BIT_EXE_READ_CODE			DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_ER // 32bit execute read code segment
#define  DA_32BIT_EXE_CONFORM_CODE		DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_EO_CF // 32 bit execute only conform code segment
#define  DA_32BIT_EXE_READ_CONFORM_CODE		DA_CODE_32BIT | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_ER_CF // 32 bit execute read conform code segment

#define  DA_16BIT_EXE_ONLY_CODE			DA_CODE_16BIT | DA_PRESENT | DA_CATEGORY_DC| DA_TYPE_EO

// Different Data Segments
#define  DA_READ_ONLY_DATA		DA_CATEGORY_DC | DA_PRESENT | DA_TYPE_RO | DA_DATA_4G_UPPER_LIMIT // read only data segment
#define  DA_READ_WRITE_DATA		DA_CATEGORY_DC | DA_PRESENT | DA_TYPE_RW | DA_DATA_4G_UPPER_LIMIT // read write data segment
#define  DA_READ_WRITE_ACCESSED_DATA		DA_CATEGORY_DC | DA_PRESENT | DA_TYPE_RW_ACCESSED | DA_DATA_4G_UPPER_LIMIT // read write accessed data segment

// Different Stack Segments
#define  DA_32BIT_READ_WRITE_STACK		DA_STACK_USE_ESP | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_RW // read write stack segment
#define  DA_32BIT_READ_WRITE_ACCESSED_STACK		DA_STACK_USE_ESP | DA_PRESENT | DA_CATEGORY_DC | DA_TYPE_RW_ACCESSED // read write accessed stack segment


//---------------------------------- Memory Paging Macros ---------------------------------------
#define  PG_PRESENT				1
#define  PG_READ_ONLY				0
#define  PG_READ_WRITE				2
#define  PG_US_SUPER				0
#define  PG_US_USER				4
#define  PG_PWT_WB				0 	// Page Cache Strategy - Write Back
#define  PG_PWT_WT				8	// Page Cache Strategy - Write Through
#define  PG_PCD_ENABLE				0	// Page Cachable ? True
#define  PG_PCD_DISABLE				0x10	// Page Cachable ? False
#define  PG_ACCESSED				0x20	
#define  PG_DIRTY				0x40	// Apply to page table entry only
#define  PG_PS_4KB				0	// Apply to page directory entry only	
#define  PG_PAT					0	// We don't use this, set to 0
#define  PG_GLOBAL				0x100	//
