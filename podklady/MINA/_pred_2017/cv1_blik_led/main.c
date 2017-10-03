#include <stm32f10x.h>  // v podstate jediny nutny include, obsahuje nazvy registru, bitu, ...

// LED na desce je pripojena na 5. bit IO brany A (= GPIOA5)

int main(void)
{
	int x;      // pomocna obecna promenna, zde pouzita jen pro dummy-pocitadlo
	
  SystemCoreClockUpdate();
  
  if (!(RCC->APB2ENR & RCC_APB2ENR_IOPAEN))   // test zda je periferie povolena (= ma pripojene hodiny)
  {                                           // a kdyz neni, musi se povolit a vyresetovat
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;       // povolit hodiny pro GPIOA - je na sbernici APB2
    RCC->APB2RSTR |= RCC_APB2RSTR_IOPARST;    // maskovanim nastavi RESET bit periferie do 1
    RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPARST;   // maskovanim nastavi RESET bit periferie do 0 = dohromady udela RESET puls
  }
  
  GPIOA->CRL &= ~(0x0f << (4 * 5));           // kazdy IO bit je rizen 4-bity v registru CRL (pro nizsich 8 IO)
      // 1111 posunuta o 5 odpovida 0000 0000 1111 0000 0000 0000 0000 0000
      // bitovou negaci pak ziskame 1111 1111 0000 1111 1111 1111 1111 1111
      // a maskovanim se prislusne 4-bity vynuluji beze zmeny ostatnich
  GPIOA->CRL |= (0x03 << (4 * 5));            // hodnota 0011 pro 4 bity ridici GPIOA5
      // vystupni rezim, strmost hrany 50MHz, push-pull mod

	while(1)
	{
    GPIOA->ODR ^= (1 << 5);                   // v datovem registru odpovida jednomu IO jeden bit
      // XOR-em menime 0 na 1 a naopak = zmena stavu
		for (x = 0; x < 1000000; x++)             // cekani prazdnym cyklovanim
			;
    
    GPIOA->ODR ^= (1 << 5);
		for (x = 0; x < 2000000; x++)             // jina doba cekani = lisi se doba sviti/nesviti LED
			;
	}
}
