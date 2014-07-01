/*
    Copyright (C) 2013 Catron
*/

#ifndef PHY_SIM_H
#define PHY_SIM_H

void rn_phy_send_port(uint16_t send_port);
void rn_phy_receive_port(uint16_t send_port);

void send_to_port(msg_buf_t *transport);

#endif /* PHY_SIM_H */



