#include<lib/ringbuffer.h>
#include<kernel/handler.h>

#define Base (0x7E201000 - 0x3F000000 )
#define UART_FR_BUSY (1 << 3)
#define UART_FR_RXFE (1 << 4)
#define UART_FR_TXFF (1 << 5)
#define UART_CR_UARTEN (1 << 0)
#define UART_CR_TXE (1 << 8)
#define UART_CR_RXE (1 << 9)
#define UART_LCRH_FEN (1 << 4)
#define RXIM (1<<4)


struct uart {
    unsigned int dr;
    unsigned int rsrecr;
    unsigned int unused[4] ;

    unsigned int fr ;
    unsigned int unused_1[1] ;

    unsigned int ILPR ;
    unsigned int IBRD;
    unsigned int FBRD;
  	unsigned int LCRH;
  	unsigned int CR;
  	unsigned int IFLS;
  	unsigned int IMSC;
    unsigned int RIS;
    unsigned int MIS ;
    unsigned int ICR ;
    unsigned int DMACR;
    unsigned int unused_2[13] ;
    unsigned int ITCR;
    unsigned int ITIP;
    unsigned int ITOP;
    unsigned int TDR;

};

volatile  struct uart *  new_uart = (struct uart *) Base ;


int is_busy (){
  return(new_uart->fr & UART_FR_BUSY);
}

int transmit_fifo_full (){
  return(new_uart->fr & UART_FR_TXFF);
}

int receive_fifo_empty (){
  return (new_uart->fr & UART_FR_RXFE) ;
}


int uart_vorbi (int aufgabe){
    //Schritte : 1. Disable the UART.
    new_uart->CR = new_uart->CR & ~ UART_CR_UARTEN ;
    //2. Wait for the end of transmission or reception of the current character.
    while (is_busy() == 1){} ;
    new_uart->CR = new_uart->CR & ~ UART_CR_TXE ;
    new_uart->CR = new_uart->CR & ~ UART_CR_RXE;
    //3. Flush the transmit FIFO by setting the FEN bit to 0 in the Line Control Register, UART_LCRH.
    new_uart->LCRH = new_uart->LCRH & ~ UART_LCRH_FEN ;

    //4. Reprogram the Control Register, UART_CR.
    //new_uart->CR = 0x0 ;
    switch (aufgabe) {
      case 1 : // 1 for transmit
          new_uart->CR |= UART_CR_TXE;
          break;
      case 2 : // 2 for receive
          new_uart->CR |= UART_CR_RXE;
          break;
      default:
          return -1 ;
  }
  //5. Enable the UART and FIFO.
    new_uart->CR = new_uart->CR |  UART_CR_UARTEN ;
    new_uart->LCRH = new_uart->LCRH | UART_LCRH_FEN ;


  return 0 ;
}


void transmit(char t) {

  while (transmit_fifo_full()) {}
  while (is_busy()){}

  new_uart->dr = t ;

}

char recieve () {
  while (receive_fifo_empty()){}
  while (is_busy()){}
  char x = new_uart->dr;
  return x ;
}


int deaktivate_fifo (int ctrl){

	if ( (ctrl == 0) || (ctrl == 1)) {
  		return ctrl ;
  	} else {
      return -1 ; // error
    }
}

void interrupt(int ctrl){
  if (ctrl==1)  {
    new_uart->IMSC  |= RXIM ;                           //interrupt modus recieve
    new_uart->LCRH = new_uart->LCRH & ~ UART_LCRH_FEN ; //fifo aus
  }
}


