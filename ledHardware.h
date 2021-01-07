#ifndef LEDHARDWARE_H_INCLUDED
#define LEDHARDWARE_H_INCLUDED

/* LED auf Basisplatine */
#define LEDROT_PORT PORTA
#define LEDROT_PIN  PIN2_bm
#define LEDGRUEN_PORT PORTA
#define LEDGRUEN_PIN  PIN3_bm


#define LEDROTSETUP LEDROT_PORT.DIRSET=LEDROT_PIN
#define LEDROT_ON   LEDROT_PORT.OUTCLR=LEDROT_PIN
#define LEDROT_OFF  LEDROT_PORT.OUTSET=LEDROT_PIN
#define LEDROT_TOGGLE  LEDROT_PORT.OUTTGL=LEDROT_PIN

#define LEDGRUENSETUP LEDGRUEN_PORT.DIRSET=LEDGRUEN_PIN
#define LEDGRUEN_ON   LEDGRUEN_PORT.OUTCLR=LEDGRUEN_PIN
#define LEDGRUEN_OFF  LEDGRUEN_PORT.OUTSET=LEDGRUEN_PIN
#define LEDGRUEN_TOGGLE  LEDGRUEN_PORT.OUTTGL=LEDGRUEN_PIN

#endif // LEDHARDWARE_H_INCLUDED
