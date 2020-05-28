/* Configuration for tasking */
#ifndef IFX_CFG_H
#define IFX_CFG_H
/* necessary to set up iLLD properly */
#define IFX_CFG_USE_COMPILER_DEFAULT_LINKER (1)
// XTAL/PLL frequency
#define IFX_CFG_SCU_XTAL_FREQUENCY (20000000)
#define IFX_CFG_SCU_PLL_FREQUENCY (200000000)
// Assertions
#define IFX_ASSERT(level, expr)
#endif /* IFX_CFG_H */
