//Jessica Wozniak & Ryan Hare
//Lab 5 Sensors: ADC12 MSP430FR6989- Temp Sense
//Created: 11/7/17
//Last updated: 11/15/17

#include <msp430.h>

#define ADC12 BIT4          //define ADC12 as BIT4
#define LED1 BIT0           //define LED1 as BIT0
#define RXD BIT0            //define RXD as BIT0
#define TXD BIT1            //define TXD as BIT1

void TimerInit(void);      //Timer function
void ADC12Init(void);      //ADC10 function
void UARTInit(void);       //UART function
void GPIOInit(void);       //GPIO function
void ClockInit(void);      //clock function

unsigned int in;
float voltage;
float tempC;
float tempF;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  ADC12Init();                  //ADC10 Function
  TimerInit();                  //Timer Function
  UARTInit();                   //UART Function
  GPIOInit();                   //GPIO Function
  ClockInit();

  PM5CTL0 &= ~LOCKLPM5;         //disable HIGH-Z mode

  __bis_SR_register(GIE);      // Global Interrupt Enable
  while(1){
  }
}
//ADC ISR
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    in = ADC12MEM0;
    voltage = in * 0.000234;        //converts ADC to voltage
    tempC= voltage/ 0.01;           //converts voltage to Temp C
    tempF=((9*tempC)/5)+32;             //Temp C to Temp F

    while(!(UCA0IFG&UCTXIFG));      //wait for TX to clear
    UCA0TXBUF = tempF;              //send to TX
}

//Timer ISR
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)

{
    ADC12CTL0 |= ADC12SC | ADC12ENC;         //sample/ enable
}
//ADC Initialization
void ADC12Init()
{
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
    ADC12CTL1 = ADC12SHP;                     // ADCCLK = MODOSC; sampling timer
    ADC12CTL2 |= ADC12RES_2;                  // 12-bit conversion results
    ADC12IER0 |= ADC12IE0;                    // Enable ADC conv complete interrupt
    ADC12MCTL0 |= ADC12INCH_4;                // A1 ADC input select
    P1OUT = LED1;                             // LED1 on
}
// Timer Initialization
void TimerInit()
{
    TA0CCTL0 = CCIE;                           // Enable interrupt
    TA0CCR0 = 4096-1;                           // PWM Period
    TA0CCTL1 = OUTMOD_3;                       // TACCR1 set/reset
    TA0CCR1 = 256;                             // TACCR1 PWM Duty Cycle
    TA0CTL = TASSEL_1 + MC_1 + ID_3;                   // ACLK, CONT MODE
}
void ClockInit()
{
    CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL0_H = 0;                             // Lock CS registers
}
//UART Initialization
void UARTInit()
{
      P2SEL0 |= RXD | TXD;                    // P2.0 = RXD, P2.1=TXD
      P2SEL1 &= ~(RXD | TXD);                 // P2.0 = RXD, P2.1=TXD

      UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
      UCA0CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
      UCA0BR0 = 52;                             // 8000000/16/9600
      UCA0BR1 = 0;
      UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
      UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
}
void GPIOInit(void)
{
    // GPIO Setup
    P1OUT &= ~BIT0;                           // Clear LED to start
    P1DIR |= BIT0;                            // P1.0 output
    P1SEL1 |= ADC12;                          // P1.4 for ADC
    P1SEL0 |= ADC12;
}
