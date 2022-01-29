#pragma once

#include "../IO.h"
#include "../Base.h"
#include "InterruptController.h"

#define PIC_MASTER_CMD_PORT                        0x0020
#define PIC_MASTER_DATA_PORT                       0x0021
#define PIC_SLAVE_CMD_PORT                        0x00a0
#define PIC_SLAVE_DATA_PORT                       0x00a1

#define ICW1_ICW4                       0x01        // ICW4 command word: 0 = not needed, 1 = needed
#define ICW1_SINGLE                     0x02        // Single mode: 0 = cascade, 1 = single
#define ICW1_ADI                        0x04        // Call address interval: 0 = interval of 8, 1 = interval of 4
#define ICW1_LTIM                       0x08        // Interrupt trigger mode: 0 = edge, 1 = level
#define ICW1_INIT                       0x10        // Initialization

#define ICW4_8086                       0x01        // Microprocessor mode: 0=MCS-80/85, 1=8086/8088
#define ICW4_AUTO                       0x02        // Auto EOI: 0 = disabled, 1 = enabled
#define ICW4_BUF_SLAVE                  0x04        // Buffered mode/slave
#define ICW4_BUF_MASTER                 0x0C        // Buffered mode/master
#define ICW4_SFNM                       0x10        // Special fully nested is programmed


class PIC {
    public:
        enum PIC_TYPE {
            MASTER,
            SLAVE
        };

        u8 m_cached_irq_mask;
        PIC(u8 remaping_base = 0x50);

        constexpr u16 cmd_port(PIC_TYPE);
        constexpr u16 data_port(PIC_TYPE);

        void init(u8);
        void write_cmd_port(PIC_TYPE, u8);
        u8 read_cmd_port(PIC_TYPE);
        void write_data_port(PIC_TYPE, u8);
        u8 read_data_port(PIC_TYPE);
        PIC_TYPE irq_to_pic_type(u8);
        void enable_vector(u8);
        void disable_vector(u8);
        u8 get_interrupt_mask(PIC_TYPE);
        void send_eoi(u8);


};
