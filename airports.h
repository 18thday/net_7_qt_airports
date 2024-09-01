#ifndef AIRPORTS_H
#define AIRPORTS_H

#include <QString>


struct Airport {
    QString code;
    QString airport_name;
};

struct FlightInfo {
    QString flight_no;
    QString scheduled_time;
    QString airport_name;
};

#endif // AIRPORTS_H
