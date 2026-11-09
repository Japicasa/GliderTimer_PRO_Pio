
/* Generated from GenMyModel */

#include "Myudp.h"
#include "KeeperESP32WIFI.h"
extern Timer_flight_data FLTdata;
extern GTimer GT;
extern AsyncUDP udp;
//extern bool STA_connected;
void Myudp::begin()
{
    clear_Myudp();

    // // tc.toCharArray(replyPacket, 50);

    // // Udp.begin(localUdpPort);
    // if (Udp.listen(localUdpPort))
    // {
    //     Serial.print("UDP Listening on IP: ");
    //     Serial.println(WiFi.localIP());
    //     Udp.onPacket([](AsyncUDPPacket packet) {
    //         handleUdp(packet);
    //         Serial.print("UDP Packet Type: ");
    //         Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast"
    //                                                                                : "Unicast");
    //         Serial.print(", From: ");
    //         Serial.print(packet.remoteIP());
    //         Serial.print(":");
    //         Serial.print(packet.remotePort());
    //         Serial.print(", To: ");
    //         Serial.print(packet.localIP());
    //         Serial.print(":");
    //         Serial.print(packet.localPort());
    //         Serial.print(", Length: ");
    //         Serial.print(packet.length());
    //         Serial.print(", Data: ");
    //         Serial.write(packet.data(), packet.length());
    //         Serial.println();
    //         //reply to the client
    //         //packet.printf("Got %u bytes of data", packet.length());
    //         // memcpy(incomingPacket, packet.data(), packet.length());
    //         String incoming_package = packet.readString();
    //         //sprintf(&incomingPacket, const char *format-string, argument-list);
    //     });

    //     Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
    // }
}

void Myudp::handleUdp()
{

    //incoming_package = String(incomingPacket);

    extract_pack_times();                             //incoming package will be sliced
    round_trip = millis_in - timer_out_millis_inpack; //millis_in comes from Lamda function
                                                      // Serial.print("UDP packet contents; %s ");
    Serial.print(incomingPacket);
    Serial.printf("->UDP ouT: ;%d; round trip has been;%d;and millis;%d\n", millis_out, round_trip, millis());
    if (round_trip < GOOD_TRIP_MS) //we get a non round trip lost pack
    {
        evaluate_timeshift();
    }
    else
    {
        packet_lost++;
        Serial.print("packet: ");
        Serial.print(seq);
        Serial.print("  is packet lost#: ");
        Serial.println(packet_lost);
        if (packet_lost > MAX_PACKET_LOST)
        {
            FLTdata.tfd.timer_to_keeper_sec = 0;
            FLTdata.tfd.timer_to_keeper_usec = 0;
            FLTdata.timer_to_keeper.tv_sec = 0;
            FLTdata.timer_to_keeper.tv_usec = 0;
            syncro_status = FAILED_SYCHRONISING;
            seq = 1;
        }
    }
    mypacket_received = 1;
}

void Myudp::return_timeshift(bool STA_connected)
{
    //String mytrag = "http://" + TFD.keeperIP + "/last_flight";
    // Serial.print("entro2 time shift");Serial.println(millis());

    uint32_t counter = millis(); //this is to bypas a potential late web response, may be useless
    while (wait_flag)
    {
        if ((millis() - counter) > 10000)
        {
            Serial.print("TIMEOUT_WAIT_FLAG");
            GT.manage_error(ERROR_SYNC);
            break;
        }
    }

    if (STA_connected)
    {
        udpAddress = IPAddress(192, 168, 4, 1);        
    }
    else
    {       
        udpAddress = IPAddress(FLTdata.keeperIP[0], FLTdata.keeperIP[1], FLTdata.keeperIP[2], FLTdata.keeperIP[3]);
    }
    //Serial.println((udpAddress.toString()));

    if (((millis() - millis_out) > TIME_FOR_UDP_RETRY) || (mypacket_received))
    {
        // Serial.print("requesting to IP: ");
        // Serial.println(udpAddress);
        //delay(25);
        gettimeofday(&timer_udp_timeval, NULL);
        millis_out = millis();
        tc = ";GT;" + String(millis()) + ";" + String(seq);
        //udp.broadcast(tc.c_str());
        char mybuff[50];

        int leng = tc.length();
        tc.toCharArray(mybuff, leng + 1);

        udp.writeTo((byte *)mybuff, leng, udpAddress, localUdpPort);
        WiFi.setSleep(false); ///pruebas con el udp normal, para tira
        mypacket_received = 0;
        seq++;
    }
}

void Myudp::extract_pack_times()
{
    incoming_package = incoming_package.substring(1);
    //Serial.println(incoming_package);
    int start = incoming_package.indexOf(";");
    incoming_package = incoming_package.substring(start + 1); //eliminated GK
    //Serial.println(incoming_package);
    start = incoming_package.indexOf(";");
    keeper_udp_timeval.tv_sec = (uint32_t)(incoming_package.substring(0, start).toInt()); // Keeper seconds
    incoming_package = incoming_package.substring(start + 1);
    //Serial.println(incoming_package);
    start = incoming_package.indexOf(";");
    keeper_udp_timeval.tv_usec = (uint32_t)(incoming_package.substring(0, start).toInt()); //Keeper micros
    incoming_package = incoming_package.substring(start + 1);
    //Serial.println(incoming_package);
    start = incoming_package.indexOf(";");
    incoming_package = incoming_package.substring(start + 1); //eliminated GT
    //Serial.println(incoming_package);
    start = incoming_package.indexOf(";");
    timer_out_millis_inpack = (uint32_t)(incoming_package.substring(0, start).toInt()); //GT millis_out received from GK
    incoming_package = incoming_package.substring(start + 1);
    //Serial.println(incoming_package);
    start = incoming_package.indexOf(";");
    out_seq = (uint32_t)(incoming_package.substring(0, start).toInt()); //new seq number
}

void Myudp::evaluate_timeshift()
{

    timer_udp_timeval.tv_usec += (round_trip * 1000 / 2);
    if (timer_udp_timeval.tv_usec > 1000000)
    {
        timer_udp_timeval.tv_usec = timer_udp_timeval.tv_usec - 1000000;
        timer_udp_timeval.tv_sec++;
    }
    my_timer_to_keeper = FLTdata.compare_times(timer_udp_timeval, keeper_udp_timeval);

    if (timeshift_iteration_counter == 0)
    {
        my_reference_sec = my_timer_to_keeper.tv_sec;
        my_reference_shift = 0;
        my_reference_usec = my_timer_to_keeper.tv_usec;
    }
    else
    {
        my_reference_shift += (my_timer_to_keeper.tv_sec - my_reference_sec);
        my_reference_usec += my_timer_to_keeper.tv_usec;
    }
    timeshift_iteration_counter++;
    if (timeshift_iteration_counter >= ITERATIONS_FOR_REFERENCE)
    {
        float my_reference_shift_avg = (float)(my_reference_shift / (float)timeshift_iteration_counter);
        my_reference_shift = int(my_reference_shift_avg);
        my_timer_to_keeper.tv_sec = my_reference_sec + my_reference_shift;
        my_reference_usec = (my_reference_usec / timeshift_iteration_counter) + (uint32_t)((my_reference_shift_avg - my_reference_shift) * 1000000);
        if (my_reference_usec < 0)
        {
            my_reference_usec += 1000000;
            my_timer_to_keeper.tv_sec = my_timer_to_keeper.tv_sec - 1;
        }
        if (my_reference_usec > 1000000)
        {
            my_reference_usec = my_reference_usec - 1000000;
            my_timer_to_keeper.tv_sec = my_timer_to_keeper.tv_sec + 1;
        }
        my_timer_to_keeper.tv_usec = my_reference_usec;
        // Serial.printf("\nse acabo, my_timer_to_keeper:");
        // Serial.printf(";%d;%d\n", my_timer_to_keeper.tv_sec, my_timer_to_keeper.tv_usec);
        FLTdata.tfd.timer_to_keeper_sec = my_timer_to_keeper.tv_sec;
        FLTdata.tfd.timer_to_keeper_usec = my_timer_to_keeper.tv_usec;
        FLTdata.timer_to_keeper.tv_sec = my_timer_to_keeper.tv_sec;
        FLTdata.timer_to_keeper.tv_usec = my_timer_to_keeper.tv_usec;
        seq = 1;

        syncro_status = SUCCESS_SYCHRONISING;
    }
    // Serial.printf("ref;%d;%d//", my_reference_shift, my_reference_usec);
}

void Myudp::clear_Myudp()
{
    wait_flag = 0;
    syncro_status = NOT_SYCHRONISING;
    packet_lost = 0;
    timeshift_iteration_counter = -5;
    //seq = 1;
}
