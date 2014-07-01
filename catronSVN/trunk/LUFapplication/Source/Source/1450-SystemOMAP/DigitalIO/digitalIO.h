/*
 * Catron header
 *
 * Abstraction of the Digital IO
 *
 */

#include <xdc/std.h>

/*
 * Digital IO LED definitions
 */
#define DIO_LED_IND1_MASK (1<<9)
#define DIO_LED_IND2_MASK (1<<12)
#define DIO_LED_IND3_MASK (1<<13)
#define DIO_LED_IND4_MASK (1<<14)
#define DIO_LED_IND5_MASK (1<<15)
#define DIO_LED_ALL_MASK (DIO_LED_IND1_MASK | DIO_LED_IND2_MASK | DIO_LED_IND3_MASK | DIO_LED_IND4_MASK | DIO_LED_IND5_MASK)

/*
 * Define Digital IO interrupts
 */
#define DIO_RADIO_IRQ1 (1)
#define DIO_RADIO_IRQ2 (2)

/*
 * Return codes
 */

#define DIO_OK      (0)
#define DIO_BUSY    (1)
#define DIO_BAD_ARG (2)

/*
 * dio_init
 *
 * Description: Init digital IO.
 *              Must be called before anything else
 */
void dio_init(void);

/*
 * dio_setLED
 *
 * Description: Set MMI LED status
 * Param:       onMask: Bit pattern showing LEDs to turn on.
 *             offMask: Bit pattern showing LEDs to turn off.
 *
 */
Int8 dio_setLED(Uint16 onMask, UInt16 offMask);

/*
 * dio_getLED
 *
 * Description: Get MMI LED status
 * Param:       readMask: Bit pattern of LEDs to read.
 *
 */
Int8 dio_getLED(Uint16 readMask);

/*
 * dio_subscribeIO
 *
 * Description: Subscribe for digital events (interrupts)
 * Param:       IO - defined interrupt
 *              callback - Function to call when interrpt occur.
 *                         Will be executed within a interrupt context.
 *
 */
Uint8 dio_subscribeIO(Uint8 io, void (*callback)(Uint8 io));


