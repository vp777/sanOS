#include "PIC.h"


PIC::PIC(u8 remaping_base){
    init(remaping_base);
}

constexpr INLINE u16 PIC::cmd_port(PIC_TYPE pt){
    return pt==MASTER?PIC_MASTER_CMD_PORT:PIC_SLAVE_CMD_PORT;
}

constexpr INLINE u16 PIC::data_port(PIC_TYPE pt){
    return pt==MASTER?PIC_MASTER_DATA_PORT:PIC_SLAVE_DATA_PORT;
}

INLINE void PIC::write_cmd_port(PIC_TYPE pt, u8 data){
    out8(cmd_port(pt), data);
}

INLINE u8 PIC::read_cmd_port(PIC_TYPE pt){
    return in8(cmd_port(pt));
}

INLINE void PIC::write_data_port(PIC_TYPE pt, u8 data){
    out8(data_port(pt), data);
}

INLINE u8 PIC::read_data_port(PIC_TYPE pt){
    return in8(data_port(pt));
}

INLINE u8 PIC::get_interrupt_mask(PIC_TYPE pt){
    return read_data_port(pt);
}

INLINE PIC::PIC_TYPE PIC::irq_to_pic_type(u8 irq){
    return (irq & 8)?SLAVE:MASTER;
}

void PIC::enable_vector(u8 irq){
    u8 imr;
    PIC_TYPE pic_type = irq_to_pic_type(irq);

    imr = get_interrupt_mask(pic_type);
    imr &= ~(1 << (irq & 7));
    write_data_port(pic_type, imr);
    m_cached_irq_mask &= ~(1 << irq);
}

void PIC::disable_vector(u8 irq) {
    u8 imr;
    PIC_TYPE pic_type = irq_to_pic_type(irq);

    imr = get_interrupt_mask(pic_type);
    imr |= 1 << (irq & 7);
    write_data_port(pic_type, imr);

    m_cached_irq_mask |= 1 << irq;
}

void PIC::send_eoi(u8 irq){
    PIC_TYPE pt = irq_to_pic_type(irq);
    if (pt == SLAVE) {
        write_cmd_port(SLAVE, 60 | (irq & 7));
        write_cmd_port(MASTER, 60 | (2));
    } else {
        write_cmd_port(MASTER, 60 | (irq));
    }
}

void PIC::init(u8 offset) {
    // ICW1: start initialization, ICW4 needed
    write_cmd_port(MASTER, ICW1_INIT | ICW1_ICW4);
    write_cmd_port(SLAVE, ICW1_INIT | ICW1_ICW4);

    // ICW2: interrupt vector address
    write_data_port(MASTER, offset);
    write_data_port(SLAVE, offset + 8);

    // ICW3: master/slave wiring
    write_data_port(MASTER, 4);
    write_data_port(SLAVE, 2);

    // ICW4: 8086 mode, not special fully nested, not buffered, normal EOI
    write_data_port(MASTER, ICW4_8086);
    write_data_port(SLAVE, ICW4_8086);

    // OCW1: Disable all IRQs
    write_data_port(MASTER, 0xff);
    write_data_port(SLAVE, 0xff);

    //enable slave
    enable_vector(2);
}
