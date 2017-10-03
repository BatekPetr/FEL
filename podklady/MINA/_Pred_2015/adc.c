
void InitADCPot(void)   // PC4 = ADC14 without remapping
{
  if (!(RCC->APB2ENR & RCC_APB2ENR_IOPCEN))
  {
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
		RCC->APB2RSTR |= RCC_APB2RSTR_IOPCRST;
		RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPCRST;
  }
		
  GPIOC->CRL &= 0xffff0ffff;
  GPIOC->CRL |= (unsigned int)0x00 << (4 * 4);	// PC4 input mode analog mode
  
  if (!(RCC->APB2ENR & RCC_APB2ENR_ADC1EN))
  {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2RSTR |= RCC_APB2RSTR_ADC1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_ADC1RST;
  }
  
  ADC1->CR1 = 0;                  // all bits "off" = no ADC-WDGs, no INTs
  ADC1->CR1 |= ADC_CR1_SCAN;      // scan-mode = use SQRx registers to select channels

  ADC1->CR2 = 0;                  // ALING = 0 - right align
  ADC1->SMPR1 = (unsigned int)0x05 << (4 * 3);  // channels 17-10
                                                // 101 = 55.5 cycles, 3 bits for input
  ADC1->SMPR2 = 0;                // channels  9-0
  ADC1->SQR1 = 0;
  ADC1->SQR2 = 0;
  ADC1->SQR3 = 14;                // single channel

  ADC1->CR2 = ADC_CR2_EXTTRIG |   // enable external trigger
              ADC_CR2_EXTSEL |    // 111 EXTSEL (external trigger) = SWSTART
              ADC_CR2_ADON;       // enable ADC periph.
  
  ADC1->CR2 |= ADC_CR2_RSTCAL;    // Initialize calibration registers
  while (ADC1->CR2 & ADC_CR2_RSTCAL)  // Wait for initialization to finish
    ; 
  ADC1->CR2 |= ADC_CR2_CAL;       // Start calibration
  while (ADC1->CR2 & ADC_CR2_CAL)     // Wait for calibration to finish
    ;

  //  ADC1->CR2 |=  ADC_CR2_SWSTART;  // Start first conversion
}

unsigned int ConvertADCPot(void)
{
  ADC1->CR2 |= ADC_CR2_SWSTART;   // set to start, cleared automatically after finish
  while(!(ADC1->SR & ADC_SR_EOC)) // end-of-conversion, cleared by reading SR register
    ;
  
  return ADC1->DR;
}

