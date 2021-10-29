// Adwandler.c - Copyright 2016, HZB, ILL/SANE & ISIS
#define RELEASE_ADWAND 1.00

// HISTORY -------------------------------------------------------------
// 1.00 - First release on March 2016

#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

#include "adwandler.h"

// Initialize analog to digital converter on selected channel
void adc_init(uint8_t channel)
{
	//	uint16_t result;

	// ADCSRA means ADC Control and Status Register A
	// Set ADEN to true enables the ADC
	// Set the division factor to 128 between the XTAL frequency and the input clock to the ADC
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);

	ADMUX = channel;					// Select channel
	ADMUX |= (0<<REFS1) | (1<<REFS0); 	// Intern reference on AVCC with external capacitor at AREF pin
	
	// First conversion performs initialization of ADC
	ADCSRA |= (1<<ADSC);
	
	while (ADCSRA & (1<<ADSC));

	//	result = ADCW;
}

///rounds double number
double round_double(double number, int digits)
{	int temp;

	int mul = pow(10,digits);
	number*=mul;
	if(number>0)
	number+=0.5;
	else
	number-=0.5;
	//cut off
	temp=number;
	number=temp;
	
	number/=mul;
	return number;
}

double map_to_current(double adcVal)   //???
{
	//	return ((adcVal)/((1023.0*10.0)/(100.0*11.0)));
	return adcVal * MAP_TO_CURRENT_FACTOR + MAP_TO_CURRENT_OFFSET;
}

double map_to_volt(double adcVal)   //???
{
	//	return ((adcVal)/((1023.0*10.0)/(35.0*11.0)));
	return (adcVal * MAP_TO_VOLTAGE_FACTOR) + MAP_TO_VOLTAGE_OFFSET;
}

double map_to_default(double adcVal)
{
	return ((adcVal)/((1023.0)/1.0));
}

double map_to_test(double adcVal)
{
	return ((adcVal)/((1023.0)/50.0));
}

double map_to_batt(double adcVal)
{
	//	return ((adcVal)/((1023.0*10.0)/(15.0*11.0))*1.05);
	return adcVal * MAP_TO_BATT_FACTOR + MAP_TO_BATT_OFFSET;
}

double map_to_pres(double adcVal, double zero, double span)
{
	//	return (((adcVal)/((1023.0)/10.0))*span) - zero;
	return adcVal * MAP_TO_PRESS_FACTOR * span + zero;
}

double read_Vcc(){
	double Vcc_value ;

	Vcc_value = readChannel(0x0e,40);


	return Vcc_value	= ( 0x400 * 1.1 ) / (Vcc_value)   ;

 	
}


double calc_he_level(double res_x, double res_min, double res_max)
{
	// The measured resistance is quite high, probe is probably not connected
	if((fabs(res_x) > 5000))  // ||(fabs(res_x) < 50))
	{
		return errCode_TooHighRes;	// Return an error code
	}
	
	// The measured resistance is lower than the minimum value, the dewar is full
	if(res_x < res_min)
	{
		return 100;			// Return 100 %
	}
	
	// The measured resistance is higher than the maximum value, the dewar is empty
	if(res_x > res_max)
	{
		return 0;			// Return 0 %
	}
	
	return ((res_max - res_x)/(res_max - res_min))*100;
}



///reads the given channel and returns converted ADc-value (average of "avr" measurements)
inline uint16_t readChannel_ILM(uint8_t channel, uint8_t avg)
{
	uint16_t result = 0;
	
	ADMUX = channel;						// Kanal waehlen
	ADMUX |= (1<<REFS1) | (1<<REFS0); 	// 2,56
	
	//ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); //strom an
	
	//Messung - Mittelwert aus "avg" aufeinanderfolgenden Wandlungen
	for( uint8_t i=0; i<avg; i++ )
	{
		ADCSRA |= (1<<ADSC);            	// eine Wandlung "single conversion"
		while ( ADCSRA & (1<<ADSC) );		// auf Abschluss der Konvertierung warten
		
		result += ADCW;		    			// Wandlungsergebnisse aufaddieren
	}
	
	//ADCSRA &= ~(1<<ADEN); // ADC deaktivieren

	return result/avg;
}



///reads the given channel and returns converted ADc-value (average of "avr" measurements)
double readChannel(uint8_t mux, uint16_t avg)
{
	double result = 0;
	
	ADMUX = mux;						// Kanal waehlen
	ADMUX |= (0<<REFS1) | (1<<REFS0); 	// interne Referenz auf AVcc
	
	//Messung - Mittelwert aus "avg" aufeinanderfolgenden Wandlungen
	for( uint16_t i=0; i<avg; i++ )
	{
		ADCSRA |= (1<<ADSC);            	// eine Wandlung "single conversion"
		while ( ADCSRA & (1<<ADSC) );		// auf Abschluss der Konvertierung warten
		
		result += ADCW;		    			// Wandlungsergebnisse aufaddieren
	}

	return (double) (result/(double)avg);
}

double readChannel_calib(uint8_t channel, uint8_t nb_readings, double adczero)
{
	double result = 0;
	
	ADMUX = channel;						// Select channel
	ADMUX |= (0<<REFS1) | (1<<REFS0); 		// Internal reference on AVcc
	
	// Measurement - mean value of "nb_readings" consecutive actions
	for(uint8_t i=0; i<nb_readings; i++)
	{
		ADCSRA |= (1<<ADSC);            	// single conversion
		while ( ADCSRA & (1<<ADSC) );		// wait for the conversion is complete
		
		result += ADCW;		    			// add results conversion
	}
	
	if(((result/nb_readings) + adczero) < 0)
	{
		return 0;
	}
	
	return (((double)result/nb_readings) + adczero);
}


uint8_t get_batt_level(double batt_min, double batt_max)
{
	double batt_v = map_to_batt(readChannel(BATTERY, ADC_LOOPS));
	
	if(batt_v < batt_min)  return 0;
	else	return ((batt_v-batt_min)*100.0)/(batt_max-batt_min);
}





