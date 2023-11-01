#pragma once

#include "../Common.hpp"

class OpcodeWriter {
public:
    explicit OpcodeWriter(const string& file);

    void WriteSetRegImmediate(uint8 registerIndex, int32 value);
    void WriteSetRegAddr(uint8 registerIndex, int32 address);
    void WriteSetRegReg(uint8 destinationRegisterIndex, uint8 sourceRegisterIndex);
    void WriteSetRAddrRAddr(uint8 destinationRegisterIndex, uint8 sourceRegisterIndex);
    void WriteSetRAddrImmediate(uint8 registerIndex, int32 value);
    void WriteSetAddrImmediate(int32 address, int32 value);
    void WriteSetAddrReg(int32 address, uint8 registerIndex);
    void WriteIncReg(uint8 registerIndex);
    void WriteDecReg(uint8 registerIndex);
    void WriteJmp(int32 address);
    void WriteJmpReg(uint8 registerIndex);
    void WriteJne(int32 address);
    void WriteCmpRegImmediate(uint8 registerIndex, int32 value);
    void WriteHalt();
    void WriteAddRegRegReg(uint8 registerA, uint8 registerB, uint8 destinationRegister);
    void WriteAddRegImmReg(uint8 registerA, int32 value, uint8 destinationRegister);
    void WriteSubRegRegReg(uint8 registerA, uint8 registerB, uint8 destinationRegister);
    void WriteSubRegImmReg(uint8 registerA, int32 value, uint8 destinationRegister);
    void WriteSubImmRegReg(uint8 registerA, int32 value, uint8 destinationRegister);
    void WriteMulRegRegReg(uint8 registerA, uint8 registerB, uint8 destinationRegister);
    void WriteMulRegImmReg(uint8 registerA, int32 value, uint8 destinationRegister);
    void WriteDivRegRegReg(uint8 registerA, uint8 registerB, uint8 destinationRegister);
    void WriteDivRegImmReg(uint8 registerA, int32 value, uint8 destinationRegister);
    void WriteDivImmRegReg(uint8 registerA, int32 value, uint8 destinationRegister);

    void ReplaceInt32(uint32 position, uint32 value);

    void Flush();

    uint32 GetPosition() const;

private:
    void WriteByte(char byte);
    void WriteInt8(int8 value);
    void WriteInt16(int16 value);
    void WriteInt32(int32 value);

    string filePath;
    char* buffer;
    uint32 position;
};