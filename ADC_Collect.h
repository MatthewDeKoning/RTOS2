// ADC_Collect.h
// Works on TM4C123
// Andrew Lynch
// February 16, 2017


// -------------------- ADC_Collect -------------------- //
// Inputs: Analog in Channel  0   PE3
//                            1   PE2
//                            2   PE1
//                            3   PE0
//                            4   PD3
//                            5   PD2
//                            6   PD1
//                            7   PD0
//                            8   PE5
//                            9   PE4
//                            10  PB4
//                            11  PB5
//         FS     sample frequency in Hz (assumes 80MHz clock)
//         task   function that will be passes the data 
void ADC_Collect(uint8_t channelNum, uint32_t FS, void(*task)(unsigned long));

