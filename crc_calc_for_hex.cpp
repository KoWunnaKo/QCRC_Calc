#include <QMetaType>
#include "crc_calc_for_hex.h"





CRC_Calc_for_Hex::CRC_Calc_for_Hex(QObject *parent) :
    QObject(NULL)
{

    qRegisterMetaType<uint64_t>("uint64_t");


    this->moveToThread(&thread);

    thread.start();


    QObject::connect(&hex_to_bytes, SIGNAL(error(QString)),
                     this, SIGNAL(error(QString)) );


    QObject::connect(this, SIGNAL(run_calculate(const QString &)),
                     this, SLOT(_calculate(const QString &))  );
}



CRC_Calc_for_Hex::~CRC_Calc_for_Hex()
{
    thread.quit();
    thread.wait();
}



void CRC_Calc_for_Hex::calculate(const QString & data)
{
    emit run_calculate(data);
}



void CRC_Calc_for_Hex::_calculate(const QString &data)
{
    if( hex_to_bytes.str_to_bytes(data) != 0 )
    {
        emit calculated(0);
        return;
    }


    if(hex_to_bytes.bytes.size() == 0)
    {
        emit error("No Data");
        emit calculated(0);
        return;
    }


    if( ucrc )
        emit calculated( ucrc->get_crc(hex_to_bytes.bytes.data(), hex_to_bytes.bytes.size() ) );
}