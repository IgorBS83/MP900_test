#include <stm32f4xx.h>
	
#define PLL_M 24
#define PLL_N 336
#define PLL_P 2
#define PLL_Q 4

void mPLL_init(void)
{
	int StartUpCounter = 0;
	
	RCC->CR |= RCC_CR_HSEON;
	while(((RCC->CR & RCC_CR_HSERDY) == 0) || (++StartUpCounter < 1000)){;} 
		
/* Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

		    /* HCLK = SYSCLK / 1*/
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | 
								RCC_CFGR_PPRE2_DIV2 | 
								RCC_CFGR_PPRE1_DIV4;
		
	RCC->PLLCFGR = 	RCC_PLLCFGR_PLLSRC_HSE	|//HSE oscillator clock selected as PLL
									(PLL_Q << 24) |//PLLQ
									(((PLL_P >> 1) -1) << 16) |//PLLP
									(PLL_N << 6) |//PLLN
									PLL_M;//PLLM
		
	RCC->CR |= RCC_CR_PLLON;
		
	while((RCC->CR & RCC_CR_PLLRDY) == 0){;} 	
		
    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
  FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
		
    /* Select the main PLL as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= RCC_CFGR_SW_PLL;	
		
	while((RCC->CFGR & RCC_CFGR_SWS_1) == 0){;} 	
}

void mTimer_init()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->CR1 &= ~TIM_CR1_CEN;
//	NVIC_EnableIRQ(TIM4_IRQn);
	

//	TIM4->CNT = 0;
	TIM4->PSC = 84 - 1;		//84MHz -> 1MHz
	TIM4->ARR = 100 - 1; //1MHz  -> 10KHz			
	//TIM4->ARR = 200 - 1; //1MHz  -> 5KHz
	//TIM4->ARR = 500 - 1; //1MHz  -> 2KHz
	//TIM4->ARR = 1000 - 1; //1MHz  -> 1KHz			
  TIM4->DIER = TIM_DIER_UIE;
  TIM4->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;// | TIM_CR1_OPM;  
	TIM4->CR1 |= TIM_CR1_CEN;
//	TIM4->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;// | TIM_CR1_OPM;  
	NVIC_EnableIRQ(TIM4_IRQn);
}




int gp = 0;
int fl_100mks = 0;
int fl_10ms = 0;
int fl_diode = 0;

#define GPIO_OSPEEDER 0x8
#define GPIO_BSRR 		0x18


const int pins_out = 69;
const int pins_in = 43;

int gp_diode[1][2] = {
	{GPIOB_BASE, 5}};

int gp_out[pins_out][2] = {
	{GPIOA_BASE, 0},
	{GPIOA_BASE, 1},
	{GPIOA_BASE, 2},
	{GPIOA_BASE, 3},
	{GPIOA_BASE, 4},
	{GPIOA_BASE, 5},
	{GPIOA_BASE, 6},
	{GPIOA_BASE, 7},
	{GPIOA_BASE, 8},
	{GPIOA_BASE, 9},
	{GPIOA_BASE, 10},
	{GPIOA_BASE, 11},
	{GPIOA_BASE, 12},
	{GPIOB_BASE, 2},
	{GPIOB_BASE, 6},
	{GPIOB_BASE, 7},
	{GPIOB_BASE, 10},
	{GPIOB_BASE, 11},
	{GPIOB_BASE, 12},
	{GPIOB_BASE, 13},
	{GPIOB_BASE, 14},
	{GPIOB_BASE, 15},
	{GPIOC_BASE, 4},
	{GPIOC_BASE, 5},
	{GPIOC_BASE, 6},
	{GPIOC_BASE, 7},
	{GPIOC_BASE, 8},
	{GPIOC_BASE, 9},
	{GPIOC_BASE, 10},
	{GPIOC_BASE, 11},
	{GPIOC_BASE, 13},
	{GPIOC_BASE, 14},
	{GPIOC_BASE, 15},
	{GPIOD_BASE, 2},
	{GPIOD_BASE, 6},
	{GPIOD_BASE, 11},
	{GPIOD_BASE, 12},
	{GPIOD_BASE, 13},
	{GPIOE_BASE, 2},
	{GPIOE_BASE, 3},
	{GPIOE_BASE, 4},
	{GPIOE_BASE, 5},
	{GPIOE_BASE, 6},
	{GPIOF_BASE, 6},
	{GPIOF_BASE, 7},
	{GPIOF_BASE, 8},
	{GPIOF_BASE, 9},
	{GPIOF_BASE, 10},
	{GPIOF_BASE, 11},
	{GPIOG_BASE, 6},
	{GPIOG_BASE, 7},
	{GPIOG_BASE, 8},
	{GPIOG_BASE, 10},
	{GPIOG_BASE, 11},
	{GPIOG_BASE, 12},
	{GPIOG_BASE, 13},
	{GPIOG_BASE, 14},
	{GPIOG_BASE, 15}
};

int gp_in[pins_in][2] = {
	{GPIOB_BASE, 8},
	{GPIOB_BASE, 9},
	{GPIOC_BASE, 0},
	{GPIOC_BASE, 1},
	{GPIOC_BASE, 2},
	{GPIOC_BASE, 3},
	{GPIOD_BASE, 0},
	{GPIOD_BASE, 1},
	{GPIOD_BASE, 3},
	{GPIOD_BASE, 4},
	{GPIOD_BASE, 5},
	{GPIOD_BASE, 7},
	{GPIOD_BASE, 8},
	{GPIOD_BASE, 9},
	{GPIOD_BASE, 10},
	{GPIOD_BASE, 14},
	{GPIOD_BASE, 15},
	{GPIOE_BASE, 0},
	{GPIOE_BASE, 1},
	{GPIOE_BASE, 7},
	{GPIOE_BASE, 8},
	{GPIOE_BASE, 9},
	{GPIOE_BASE, 10},
	{GPIOE_BASE, 11},
	{GPIOE_BASE, 12},
	{GPIOE_BASE, 13},	
	{GPIOE_BASE, 14},
	{GPIOE_BASE, 15},
	{GPIOF_BASE, 0},
	{GPIOF_BASE, 1},
	{GPIOF_BASE, 2},
	{GPIOF_BASE, 3},
	{GPIOF_BASE, 4},
	{GPIOF_BASE, 5},
	{GPIOF_BASE, 12},
	{GPIOF_BASE, 13},	
	{GPIOF_BASE, 14},
	{GPIOF_BASE, 15},
	{GPIOG_BASE, 0},
	{GPIOG_BASE, 1},
	{GPIOG_BASE, 2},
	{GPIOG_BASE, 3},
	{GPIOG_BASE, 4}
};



void TIM4_IRQHandler()//timer for ask sensors 
{
	TIM4->SR = 0;
	
		*(int*)(gp_out[gp][0] + GPIO_BSRR) = 1 << (gp_out[gp][1] + 16);
	if(gp + 1 < pins_out) 
		*(int*)(gp_out[gp + 1][0] + GPIO_BSRR) = 1 << gp_out[gp + 1][1];	
	else 
		*(int*)(gp_out[0][0] + GPIO_BSRR) = 1 << gp_out[0][1];	
	
	gp++;
	if(gp >= pins_out) gp = 0;
	
	fl_100mks++;
}


int mode = 0;

struct {
	unsigned short gpio_b;	
	unsigned short gpio_c;
	unsigned short gpio_d;
	unsigned short gpio_e;
	unsigned short gpio_f;
	unsigned short gpio_g;
}gpio_input, gpio_input_sum, gpio_input_mask;

void delay(int i){
	volatile int k = 0;
	while(k++ < i);
}

int main()
{	
	volatile int i;
	volatile int mistake = 0;
	volatile int one_cnt = 0;
	volatile int one_cnt_prev = 0;
	volatile long long input;
	
	delay(0x100000);

	mPLL_init();
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN 
						| RCC_AHB1ENR_GPIOBEN 
						| RCC_AHB1ENR_GPIOCEN 
						| RCC_AHB1ENR_GPIODEN 
						| RCC_AHB1ENR_GPIOEEN 
						| RCC_AHB1ENR_GPIOFEN 
						| RCC_AHB1ENR_GPIOGEN;
	mTimer_init();
	
	*(int*)(gp_diode[0][0]) 								|= 1 << gp_diode[0][1] * 2;//mode
	*(int*)(gp_diode[0][0] + GPIO_OSPEEDER) |= 1 << gp_diode[0][1] * 2;//speed
	
	for(i = 0; i < pins_out; i++) {
		*(int*)(gp_out[i][0]) 								|= 1 << gp_out[i][1] * 2;//mode
		*(int*)(gp_out[i][0] + GPIO_OSPEEDER) |= 1 << gp_out[i][1] * 2;//speed
	}
	
	
	for(i = 0; i < pins_in; i++) {
		*(int*)(gp_in[i][0] + GPIO_OSPEEDER) |= 1 << gp_in[i][1] * 2;//speed
		switch(gp_in[i][0]){
			case GPIOB_BASE:
				gpio_input_mask.gpio_b |= 1 << gp_in[i][1];
				break;
			case GPIOC_BASE:
				gpio_input_mask.gpio_c |= 1 << gp_in[i][1];
				break;
			case GPIOD_BASE:
				gpio_input_mask.gpio_d |= 1 << gp_in[i][1];
				break;
			case GPIOE_BASE:
				gpio_input_mask.gpio_e |= 1 << gp_in[i][1];
				break;
			case GPIOF_BASE:
				gpio_input_mask.gpio_f |= 1 << gp_in[i][1];
				break;
			case GPIOG_BASE:
				gpio_input_mask.gpio_g |= 1 << gp_in[i][1];
				break;
		}
	}
	

	while(1)
	{
		if(fl_100mks >= 100)
		{
			fl_10ms++;
			fl_100mks = 0;
			if(gpio_input_sum.gpio_b != gpio_input_mask.gpio_b) 
				mistake++;
			if(gpio_input_sum.gpio_c != gpio_input_mask.gpio_c) 
				mistake++;
			if(gpio_input_sum.gpio_d != gpio_input_mask.gpio_d) 
				mistake++;
			if(gpio_input_sum.gpio_e != gpio_input_mask.gpio_e) 
				mistake++;
			if(gpio_input_sum.gpio_f != gpio_input_mask.gpio_f) 
				mistake++;
			if(gpio_input_sum.gpio_g != gpio_input_mask.gpio_g) 
				mistake++;
			if(fl_10ms > 50){
				fl_10ms = 0;
				if(mistake > 0) *(int*)(gp_diode[0][0] + GPIO_BSRR) = 1 << (gp_diode[0][1] + 16); //reset diode
				else if(fl_diode) {
								fl_diode = 0;
								*(int*)(gp_diode[0][0] + GPIO_BSRR) = 1 << (gp_diode[0][1] + 16); //reset diode
							}
							else{
								fl_diode = 1;
								*(int*)(gp_diode[0][0] + GPIO_BSRR) = 1 << (gp_diode[0][1]); //set diode
							}
			}
			gpio_input_sum.gpio_b = 0;
			gpio_input_sum.gpio_c = 0;
			gpio_input_sum.gpio_d = 0;
			gpio_input_sum.gpio_e = 0;
			gpio_input_sum.gpio_f = 0;
			gpio_input_sum.gpio_g = 0;
		}
		gpio_input.gpio_b = GPIOB->IDR & gpio_input_mask.gpio_b;
		gpio_input.gpio_c = GPIOC->IDR & gpio_input_mask.gpio_c;
		gpio_input.gpio_d = GPIOD->IDR & gpio_input_mask.gpio_d;
		gpio_input.gpio_e = GPIOE->IDR & gpio_input_mask.gpio_e;
		gpio_input.gpio_f = GPIOF->IDR & gpio_input_mask.gpio_f;
		gpio_input.gpio_g = GPIOG->IDR & gpio_input_mask.gpio_g;
		gpio_input_sum.gpio_b |= gpio_input.gpio_b;
		gpio_input_sum.gpio_c |= gpio_input.gpio_c;
		gpio_input_sum.gpio_d |= gpio_input.gpio_d;
		gpio_input_sum.gpio_e |= gpio_input.gpio_e;
		gpio_input_sum.gpio_f |= gpio_input.gpio_f;
		gpio_input_sum.gpio_g |= gpio_input.gpio_g;
		
		input = *(unsigned long long*)&gpio_input;
		one_cnt = 0;
		for(i = 0; i < 64; i++){
			one_cnt += input & 1;
			input >>= 1;
		}
		if(one_cnt > 1 && one_cnt_prev > 1) 
			mistake++;	
		one_cnt_prev = one_cnt;
	}
	return 0;
}
