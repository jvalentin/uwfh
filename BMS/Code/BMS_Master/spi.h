/*
 *SPI.h
 */
 
// Public Function prototypes
extern 	void 			spi_init( void );
extern 	void			spi_transmit( unsigned char data );
extern 	unsigned char 	spi_exchange( unsigned char data );

extern void				spi_set_mode ( unsigned char mode, unsigned char br1, unsigned char br0 ); 

// Private Function prototypes
