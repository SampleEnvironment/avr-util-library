
extern "C" {
    #include "../DS3231M.h"
    #include "../TestMocks/Mock_I2C_utilities.h"
    #include "../TestMocks/Mock_main.h"
}

#include <gtest/gtest.h>
#include <time.h>

struct  DS3231M_Test
    :public ::testing::Test
{

    struct tm ServerTime;

    uint8_t decodeDS3231M(uint8_t element){
       return (element & 0x0F) + (element >> 4)*10;
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
        setServer(30,30,12,15,7,21);

        DS_state.connected = 1;
        DS_state.error = 0;
    }

    virtual void TearDown() override{

    }
};



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


TEST_F(DS3231M_Test,TWI_not_Connected){
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