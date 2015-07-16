#ifdef SPI_DEF
#define SPI_EXT
#else
#define SPI_EXT extern
#endif

SPI_EXT void spi_init();
SPI_EXT void opamp_init();