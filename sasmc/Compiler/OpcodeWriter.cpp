#include "OpcodeWriter.hpp"
#include "../../shared/stark1-opcodes.h"

OpcodeWriter::OpcodeWriter(const string& file) {
    filePath = file;
    position = 0;
    buffer = (char*) malloc(sizeof(char) * 4096);
}

void OpcodeWriter::WriteSetRegImmediate(uint8 registerIndex, int32 value) {
    if (value <= 255) {
        WriteByte(OP_SET_REG_IMMEDIATE8);
        WriteInt8(registerIndex);
        WriteInt8(value);
    } else if (value > 255 && value <= 65536) {
        WriteByte(OP_SET_REG_IMMEDIATE16);
        WriteInt8(registerIndex);
        WriteInt16(value);
    } else {
        WriteByte(OP_SET_REG_IMMEDIATE32);
        WriteInt8(registerIndex);
        WriteInt32(value);
    }
}

void OpcodeWriter::WriteSetRegAddr(uint8 registerIndex, int32 address) {
    WriteByte(OP_SET_REG_ADDR);
    WriteInt8(registerIndex);
    WriteInt32(address);
}

void OpcodeWriter::WriteSetRegReg(uint8 destinationRegisterIndex, uint8 sourceRegisterIndex) {
    WriteByte(OP_SET_REG_REG);
    WriteInt8(destinationRegisterIndex);
    WriteInt8(sourceRegisterIndex);
}

void OpcodeWriter::WriteSetRAddrRAddr(uint8 destinationRegisterIndex, uint8 sourceRegisterIndex) {
    WriteByte(OP_SET_RADDR_RADDR);
    WriteInt8(destinationRegisterIndex);
    WriteInt8(sourceRegisterIndex);
}

void OpcodeWriter::WriteSetRAddrImmediate(uint8 registerIndex, int32 value) {
    if (value <= 255) {
        WriteByte(OP_SET_RADDR_IMMEDIATE8);
        WriteInt8(registerIndex);
        WriteInt8(value);
    } else if (value > 255 && value <= 65536) {
        WriteByte(OP_SET_RADDR_IMMEDIATE16);
        WriteInt8(registerIndex);
        WriteInt16(value);
    } else {
        WriteByte(OP_SET_RADDR_IMMEDIATE32);
        WriteInt8(registerIndex);
        WriteInt32(value);
    }
}

void OpcodeWriter::WriteSetAddrImmediate(int32 address, int32 value) {
    if (value <= 255) {
        WriteByte(OP_SET_ADDR_IMMEDIATE8);
        WriteInt32(address);
        WriteInt8(value);
    } else if (value > 255 && value <= 65536) {
        WriteByte(OP_SET_ADDR_IMMEDIATE16);
        WriteInt32(address);
        WriteInt16(value);
    } else {
        WriteByte(OP_SET_ADDR_IMMEDIATE32);
        WriteInt32(address);
        WriteInt32(value);
    }
}

void OpcodeWriter::WriteSetAddrReg(int32 address, uint8 registerIndex) {
    WriteByte(OP_SET_ADDR_REG);
    WriteInt32(address);
    WriteInt8(registerIndex);
}

void OpcodeWriter::WriteIncReg(uint8 registerIndex) {
    WriteByte(OP_INCREMENT);
    WriteInt8(registerIndex);
}

void OpcodeWriter::WriteDecReg(uint8 registerIndex) {
    WriteByte(OP_DECREMENT);
    WriteInt8(registerIndex);
}

void OpcodeWriter::WriteJmp(int32 address) {
    WriteByte(OP_JMP_ABSOLUTE);
    WriteInt32(address);
}

void OpcodeWriter::WriteJmpReg(uint8 registerIndex) {
    WriteByte(OP_JMP_REG);
    WriteInt8(registerIndex);
}

void OpcodeWriter::WriteJne(int32 address) {
    WriteByte(OP_JMP_IF_NOT_EQUAL);
    WriteInt32(address);
}

void OpcodeWriter::WriteCmpRegImmediate(uint8 registerIndex, int32 value) {
    WriteByte(OP_CMP_REG_IMMEDIATE);
    WriteInt8(registerIndex);
    WriteInt32(value);
}

void OpcodeWriter::WriteHalt() {
    WriteByte(OP_HALT);
}

void OpcodeWriter::WriteAddRegRegReg(uint8 registerA, uint8 registerB, uint8 destinationRegister) {
    WriteByte(OP_ADD_REG_REG);
    WriteInt8(registerA);
    WriteInt8(registerB);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::WriteAddRegImmReg(uint8 registerA, int32 value, uint8 destinationRegister) {
    WriteByte(OP_ADD_REG_IMM32);
    WriteInt8(registerA);
    WriteInt32(value);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::WriteSubRegRegReg(uint8 registerA, uint8 registerB, uint8 destinationRegister) {
    WriteByte(OP_SUB_REG_REG);
    WriteInt8(registerA);
    WriteInt8(registerB);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::WriteSubRegImmReg(uint8 registerA, int32 value, uint8 destinationRegister) {
    WriteByte(OP_SUB_REG_IMM32);
    WriteInt8(registerA);
    WriteInt32(value);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::WriteSubImmRegReg(uint8 registerA, int32 value, uint8 destinationRegister) {
    WriteByte(OP_SUB_IMM32_REG);
    WriteInt8(registerA);
    WriteInt32(value);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::WriteMulRegRegReg(uint8 registerA, uint8 registerB, uint8 destinationRegister) {
    WriteByte(OP_MUL_REG_REG);
    WriteInt8(registerA);
    WriteInt8(registerB);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::WriteMulRegImmReg(uint8 registerA, int32 value, uint8 destinationRegister) {
    WriteByte(OP_MUL_REG_IMM32);
    WriteInt8(registerA);
    WriteInt32(value);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::WriteDivRegRegReg(uint8 registerA, uint8 registerB, uint8 destinationRegister) {
    WriteByte(OP_DIV_REG_REG);
    WriteInt8(registerA);
    WriteInt8(registerB);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::WriteDivRegImmReg(uint8 registerA, int32 value, uint8 destinationRegister) {
    WriteByte(OP_DIV_REG_IMM32);
    WriteInt8(registerA);
    WriteInt32(value);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::WriteDivImmRegReg(uint8 registerA, int32 value, uint8 destinationRegister) {
    WriteByte(OP_DIV_IMM32_REG);
    WriteInt8(registerA);
    WriteInt32(value);
    WriteInt8(destinationRegister);
}

void OpcodeWriter::ReplaceInt32(uint32 pos, uint32 value) {
    buffer[pos + 0] = value;
    buffer[pos + 1] = value >> 8;
    buffer[pos + 2] = value >> 16;
    buffer[pos + 3] = value >> 24;
}

void OpcodeWriter::WriteByte(char byte) {
    buffer[position] = byte;
    position += 1;
}

void OpcodeWriter::WriteInt8(int8 value) {
    buffer[position] = value;
    position += 1;
}

void OpcodeWriter::WriteInt16(int16 value) {
    buffer[position++] = value;
    buffer[position++] = value >> 8;
}

void OpcodeWriter::WriteInt32(int32 value) {
    buffer[position++] = value;
    buffer[position++] = value >> 8;
    buffer[position++] = value >> 16;
    buffer[position++] = value >> 24;
}

void OpcodeWriter::Flush() {
    FILE *file = fopen(filePath.c_str(), "wb+");
    fwrite(buffer, 1, position, file);
    fclose(file);
}

uint32 OpcodeWriter::GetPosition() const {
    return position;
}
