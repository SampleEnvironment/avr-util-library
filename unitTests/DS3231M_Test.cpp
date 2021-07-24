
extern "C" {
    #include "../DS3231M.h"
    #include "../status.h"
    #include "../TestMocks/Mock_I2C_utilities.h"
    #include "../TestMocks/Mock_main.h"
    #include "../TestMocks/Mock_LCD.h"
}

#include <gtest/gtest.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct  DS3231M_Test
    :public ::testing::Test
{

    struct tm ServerTime;

    uint8_t decodeDS3231M(uint8_t element){
       return (element & 0x0F) + (element >> 4)*10;
    }

    uint8_t encodeDS3231M(uint8_t element){
       return ((element / 10) << 4)  | (element % 10);
    }


int set_env(const char *name, const char *value)
{
#ifdef _WIN32
    return _putenv_s(name, value == NULL ? "" : value);
#else
    if (value == NULL)
        return unsetenv(name);
    else
        return setenv(name, value, 1);
#endif
}

    void setServer(uint8_t sec, uint8_t min, uint8_t hour,uint8_t mday, uint8_t mon,uint8_t year){
        ServerTime.tm_sec = sec;
        ServerTime.tm_min = min;
        ServerTime.tm_hour = hour;
        ServerTime.tm_mday = mday;
        ServerTime.tm_mon = mon;
        ServerTime.tm_year = year;
    }






    virtual void SetUp() override{
        
        const char *Germany = "GST+1GDT";

        set_env("TZ", Germany);
        tzset();
           
        setServer(30,30,12,15,7,21);
    
        DS_state.connected = 1;
        DS_state.error = 0;




        DS3231M_set_time(&ServerTime);
        DS3231M_read_time();
    }

    virtual void TearDown() override{
        //reset all 
    	CLEAR_ERROR(I2C_BUS_ERROR);
		CLEAR_ERROR(TIMER_ERROR);
        connected.DS3231M = 1;
        connected.TWI = 1;
        DS_state.connected = 1;
        DS_state.error = 0;
        count_t_elapsed = 0;
    }
};


//#########################################################
// DS3231M_init() TESTS 
//#########################################################
TEST_F(DS3231M_Test,init){
    DS_state.temp[0] = 128;
    DS_state.temp[1] = 128;

    DS_state.controlReg = 128;


    uint8_t init_ret_code =  init_DS3231M(&LCD_paint_info_line);

    EXPECT_EQ(init_ret_code,0); // init success

    EXPECT_EQ(DS_state.controlReg, 128 | DS3231M_control_reg_CONV);

}

//#########################################################
// DS3231M_set_time() TESTS 
//#########################################################

TEST_F(DS3231M_Test,SetTime_mid){
    DS3231M_set_time(&ServerTime);

    EXPECT_EQ(
        ServerTime.tm_sec,
        decodeDS3231M(DS_state.date[0])
    );
    EXPECT_EQ(
        ServerTime.tm_min,
        decodeDS3231M(DS_state.date[1])
    );
    EXPECT_EQ(
        ServerTime.tm_hour,
        decodeDS3231M(DS_state.date[2])
    );
    EXPECT_EQ(
        ServerTime.tm_mday,
        decodeDS3231M(DS_state.date[4])
    );
    EXPECT_EQ(
        ServerTime.tm_mon,
        decodeDS3231M(DS_state.date[5])
    );
    EXPECT_EQ(
        ServerTime.tm_year,
        decodeDS3231M(DS_state.date[6])
    );

    EXPECT_TRUE(DS_state.connected);
    EXPECT_FALSE(DS_state.error);
}
TEST_F(DS3231M_Test,SetTime_lower){
    setServer(0,0,0,1,1,0);

    DS3231M_set_time(&ServerTime);

    EXPECT_EQ(
        ServerTime.tm_sec,
        decodeDS3231M(DS_state.date[0])
    );
    EXPECT_EQ(
        ServerTime.tm_min,
        decodeDS3231M(DS_state.date[1])
    );
    EXPECT_EQ(
        ServerTime.tm_hour,
        decodeDS3231M(DS_state.date[2])
    );
    EXPECT_EQ(
        ServerTime.tm_mday,
        decodeDS3231M(DS_state.date[4])
    );
    EXPECT_EQ(
        ServerTime.tm_mon,
        decodeDS3231M(DS_state.date[5])
    );
    EXPECT_EQ(
        ServerTime.tm_year,
        decodeDS3231M(DS_state.date[6])
    );

    EXPECT_TRUE(DS_state.connected);
    EXPECT_FALSE(DS_state.error);
}
TEST_F(DS3231M_Test,SetTime_upper){

    setServer(59,59,23,31,12,99);

    DS3231M_set_time(&ServerTime);

    EXPECT_EQ(
        ServerTime.tm_sec,
        decodeDS3231M(DS_state.date[0])
    );
    EXPECT_EQ(
        ServerTime.tm_min,
        decodeDS3231M(DS_state.date[1])
    );
    EXPECT_EQ(
        ServerTime.tm_hour,
        decodeDS3231M(DS_state.date[2])
    );
    EXPECT_EQ(
        ServerTime.tm_mday,
        decodeDS3231M(DS_state.date[4])
    );
    EXPECT_EQ(
        ServerTime.tm_mon,
        decodeDS3231M(DS_state.date[5])
    );
    EXPECT_EQ(
        ServerTime.tm_year,
        decodeDS3231M(DS_state.date[6])
    );

    EXPECT_TRUE(DS_state.connected);
    EXPECT_FALSE(DS_state.error);

}
TEST_F(DS3231M_Test,SetTime_OutOfRange){



    
    setServer(60,30,12,15,7,21);
    DS3231M_set_time(&ServerTime);

    EXPECT_NE(
        ServerTime.tm_sec,
        decodeDS3231M(DS_state.date[0])
    );
    
    setServer(30,60,12,15,7,21);
    DS3231M_set_time(&ServerTime);
    
    EXPECT_NE(
        ServerTime.tm_min,
        decodeDS3231M(DS_state.date[1])
    );

    setServer(30,30,24,15,7,21);
    DS3231M_set_time(&ServerTime);

    EXPECT_NE(
        ServerTime.tm_hour,
        decodeDS3231M(DS_state.date[2])
    );

    setServer(30,30,12,32,7,21);
    DS3231M_set_time(&ServerTime);

    EXPECT_NE(
        ServerTime.tm_mday,
        decodeDS3231M(DS_state.date[4])
    );

    setServer(30,30,12,15,13,21);
    DS3231M_set_time(&ServerTime);
    
    EXPECT_NE(
        ServerTime.tm_mon,
        decodeDS3231M(DS_state.date[5])
    );

    setServer(30,30,12,15,7,100);
    DS3231M_set_time(&ServerTime);   

    EXPECT_NE(
        ServerTime.tm_year,
        decodeDS3231M(DS_state.date[6])
    );
}
TEST_F(DS3231M_Test,SetTime_TWI_not_Connected){
    setServer(30,30,12,15,7,21);
    DS3231M_set_time(&ServerTime); 
    connected.TWI = 0;
    setServer(31,31,13,14,5,22);
    DS3231M_set_time(&ServerTime); 

    EXPECT_NE(
        ServerTime.tm_sec,
        decodeDS3231M(DS_state.date[0])
    );

    EXPECT_NE(
        ServerTime.tm_min,
        decodeDS3231M(DS_state.date[1])
    );

    EXPECT_NE(
        ServerTime.tm_hour,
        decodeDS3231M(DS_state.date[2])
    );

    EXPECT_NE(
        ServerTime.tm_mday,
        decodeDS3231M(DS_state.date[4])
    );
    EXPECT_NE(
        ServerTime.tm_mon,
        decodeDS3231M(DS_state.date[5])
    );
    EXPECT_NE(
        ServerTime.tm_year,
        decodeDS3231M(DS_state.date[6])
    );

}

//#########################################################
// DS3231M_read_time() TESTS 
//#########################################################
TEST_F(DS3231M_Test,ReadTime_mid){
        DS3231M_read_time();

        EXPECT_EQ(Time.tm_sec, 30);
        EXPECT_EQ(Time.tm_min, 30);
        EXPECT_EQ(Time.tm_hour, 12);
        
        EXPECT_EQ(Time.tm_mday, 15);
        EXPECT_EQ(Time.tm_mon, 7);
        EXPECT_EQ(Time.tm_year, 21);
}
TEST_F(DS3231M_Test,ReadTime_lower){
        setServer(0,0,0,1,1,21);
        DS3231M_set_time(&ServerTime);
        DS3231M_read_time();

        EXPECT_EQ(Time.tm_sec, 0);
        EXPECT_EQ(Time.tm_min, 0);
        EXPECT_EQ(Time.tm_hour, 0);
        
        EXPECT_EQ(Time.tm_mday, 1);
        EXPECT_EQ(Time.tm_mon, 1);
        EXPECT_EQ(Time.tm_year, 21);
    
}
TEST_F(DS3231M_Test,ReadTime_upper){
    
    setServer(59,59,23,31,12,99);

    DS3231M_set_time(&ServerTime);
    DS3231M_read_time();
    
        EXPECT_EQ(Time.tm_sec, 59);
        EXPECT_EQ(Time.tm_min, 59);
        EXPECT_EQ(Time.tm_hour, 23);
        
        EXPECT_EQ(Time.tm_mday, 31);
        EXPECT_EQ(Time.tm_mon, 12);
        EXPECT_EQ(Time.tm_year, 99);

    
}
TEST_F(DS3231M_Test,ReadTime_OutofRange_sec){
    DS_state.date[0] = encodeDS3231M(60); // DS3231M sec is defined from 0..59
    count_t_elapsed = 10;
    
    DS3231M_read_time();

    // Timer error since seconds are not in correct range
    // out of range must come from bitflip in I2C bus so I2C bus error must be set aswell
    EXPECT_TRUE(CHECK_ERROR(TIMER_ERROR));


    //TWI is still considered connected since the transmission went through without throwing any i2c error flags
    EXPECT_TRUE(connected.TWI);


    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 40);
    EXPECT_EQ(Time.tm_min, 30);
    EXPECT_EQ(Time.tm_hour, 12);
        
    EXPECT_EQ(Time.tm_mday, 15);
    EXPECT_EQ(Time.tm_mon, 7);
    EXPECT_EQ(Time.tm_year, 21);



    
}
TEST_F(DS3231M_Test,ReadTime_OutofRange_min){
    DS_state.date[1] = encodeDS3231M(60); // DS3231M min is defined from 0..59

    count_t_elapsed = 60;

    DS3231M_read_time();
    

    // Timer error since seconds are not in correct range
    // out of range must come from bitflip in I2C bus so I2C bus error must be set aswell
    EXPECT_TRUE(CHECK_ERROR(TIMER_ERROR));


    //TWI is still considered connected since the transmission went through without throwing any i2c error flags
    EXPECT_TRUE(connected.TWI);


    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 30);
    EXPECT_EQ(Time.tm_min, 31);
    EXPECT_EQ(Time.tm_hour, 12);
        
    EXPECT_EQ(Time.tm_mday, 15);
    EXPECT_EQ(Time.tm_mon, 7);
    EXPECT_EQ(Time.tm_year, 21);



    
}
TEST_F(DS3231M_Test,ReadTime_OutofRange_hour){
    DS_state.date[2] = encodeDS3231M(24); // DS3231M hour is defined from 0..23
    count_t_elapsed = 60*60;

    DS3231M_read_time();

    // Timer error since seconds are not in correct range
    // out of range must come from bitflip in I2C bus so I2C bus error must be set aswell
    EXPECT_TRUE(CHECK_ERROR(TIMER_ERROR));


    //TWI is still considered connected since the transmission went through without throwing any i2c error flags
    EXPECT_TRUE(connected.TWI);


    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 30);
    EXPECT_EQ(Time.tm_min, 30);
    EXPECT_EQ(Time.tm_hour, 13);
        
    EXPECT_EQ(Time.tm_mday, 15);
    EXPECT_EQ(Time.tm_mon, 7);
    EXPECT_EQ(Time.tm_year, 21);
}
TEST_F(DS3231M_Test,ReadTime_OutofRange_mday_below){
    DS_state.date[4] = encodeDS3231M(0); // DS3231M mday is defined from 1..31
    
    count_t_elapsed = 60*60*24;
    DS3231M_read_time();

    // Timer error since seconds are not in correct range
    // out of range must come from bitflip in I2C bus so I2C bus error must be set aswell
    EXPECT_TRUE(CHECK_ERROR(TIMER_ERROR));


    //TWI is still considered connected since the transmission went through without throwing any i2c error flags
    EXPECT_TRUE(connected.TWI);


    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 30);
    EXPECT_EQ(Time.tm_min, 30);
    EXPECT_EQ(Time.tm_hour, 12);
        
    EXPECT_EQ(Time.tm_mday, 16);
    EXPECT_EQ(Time.tm_mon, 7);
    EXPECT_EQ(Time.tm_year, 21);



    
}
TEST_F(DS3231M_Test,ReadTime_OutofRange_mday_above){
    DS_state.date[4] = encodeDS3231M(32); // DS3231M mday is defined from 1..31
   count_t_elapsed = 60*60*24*31;
    DS3231M_read_time();
    
    // Timer error since seconds are not in correct range
    // out of range must come from bitflip in I2C bus so I2C bus error must be set aswell
    EXPECT_TRUE(CHECK_ERROR(TIMER_ERROR));


    //TWI is still considered connected since the transmission went through without throwing any i2c error flags
    EXPECT_TRUE(connected.TWI);


    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 30);
    EXPECT_EQ(Time.tm_min, 30);
    EXPECT_EQ(Time.tm_hour, 12);
        
    EXPECT_EQ(Time.tm_mday, 15);
    EXPECT_EQ(Time.tm_mon, 8);
    EXPECT_EQ(Time.tm_year, 21);



    
}
TEST_F(DS3231M_Test,ReadTime_OutofRange_mon_below){
    DS_state.date[5] = encodeDS3231M(0); // DS3231M mon is defined from 1..12
    count_t_elapsed = 60*60*24*365;
    DS3231M_read_time();

    // Timer error since seconds are not in correct range
    // out of range must come from bitflip in I2C bus so I2C bus error must be set aswell
    EXPECT_TRUE(CHECK_ERROR(TIMER_ERROR));


    //TWI is still considered connected since the transmission went through without throwing any i2c error flags
    EXPECT_TRUE(connected.TWI);


    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 30);
    EXPECT_EQ(Time.tm_min, 30);
    EXPECT_EQ(Time.tm_hour, 12);
        
    EXPECT_EQ(Time.tm_mday, 15);
    EXPECT_EQ(Time.tm_mon, 7);
    EXPECT_EQ(Time.tm_year, 22);



    
}
TEST_F(DS3231M_Test,ReadTime_OutofRange_mon_above){
    DS_state.date[5] = encodeDS3231M(13); // DS3231M mon is defined from 1..12
    DS3231M_read_time();

    // Timer error since seconds are not in correct range
    // out of range must come from bitflip in I2C bus so I2C bus error must be set aswell
    EXPECT_TRUE(CHECK_ERROR(TIMER_ERROR));


    //TWI is still considered connected since the transmission went through without throwing any i2c error flags
    EXPECT_TRUE(connected.TWI);


    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 30);
    EXPECT_EQ(Time.tm_min, 30);
    EXPECT_EQ(Time.tm_hour, 12);
        
    EXPECT_EQ(Time.tm_mday, 15);
    EXPECT_EQ(Time.tm_mon, 7);
    EXPECT_EQ(Time.tm_year, 21);



    
}
TEST_F(DS3231M_Test,ReadTime_OutofRange_year_below){
    DS_state.date[6] = encodeDS3231M(20); // DS3231M year  2020  is considered out of range 
    DS3231M_read_time();

    // Timer error since seconds are not in correct range
    // out of range must come from bitflip in I2C bus so I2C bus error must be set aswell
    EXPECT_TRUE(CHECK_ERROR(TIMER_ERROR));


    //TWI is still considered connected since the transmission went through without throwing any i2c error flags
    EXPECT_TRUE(connected.TWI);


    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 30);
    EXPECT_EQ(Time.tm_min, 30);
    EXPECT_EQ(Time.tm_hour, 12);
        
    EXPECT_EQ(Time.tm_mday, 15);
    EXPECT_EQ(Time.tm_mon, 7);
    EXPECT_EQ(Time.tm_year, 21);



    
}
TEST_F(DS3231M_Test,ReadTime_OutofRange_year_above){
    DS_state.date[6] = encodeDS3231M(100); // DS3231M year  2100  is considered out of range
    DS3231M_read_time();

    // Timer error since seconds are not in correct range
    // out of range must come from bitflip in I2C bus so I2C bus error must be set aswell
    EXPECT_TRUE(CHECK_ERROR(TIMER_ERROR));


    //TWI is still considered connected since the transmission went through without throwing any i2c error flags
    EXPECT_TRUE(connected.TWI);


    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 30);
    EXPECT_EQ(Time.tm_min, 30);
    EXPECT_EQ(Time.tm_hour, 12);
        
    EXPECT_EQ(Time.tm_mday, 15);
    EXPECT_EQ(Time.tm_mon, 7);
    EXPECT_EQ(Time.tm_year, 21);



    
}
TEST_F(DS3231M_Test,ReadTime_TWI_not_Connected){
    connected.TWI = 0;
    DS3231M_read_time();

    
    // Time struct sould still hold the same values as before reading from DS3231M 
    EXPECT_EQ(Time.tm_sec, 30);
    EXPECT_EQ(Time.tm_min, 30);
    EXPECT_EQ(Time.tm_hour, 12);
        
    EXPECT_EQ(Time.tm_mday, 15);
    EXPECT_EQ(Time.tm_mon, 7);
    EXPECT_EQ(Time.tm_year, 21);
    
}


