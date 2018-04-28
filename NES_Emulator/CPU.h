#pragma once

BEGIN_NES_EMULATOR_NAMESPACE

class Console;

class CPU
{
public:

    static const uint32_t kMaxStatusStrLength = 256;

    CPU(Console* console);

    bool Step(char* statusStr = nullptr);

    void Init(uint16_t PC);

    void GetStatusStr(char* str) const;

    void FetchNextOpCode();

private:

    void AddBranchCycles();

    uint16_t Read16(uint16_t address) const;

    uint16_t Read16Bug(uint16_t address) const;

    inline void Push16(uint16_t value);

    inline void Push(uint8_t value);

    inline uint16_t Pull16();

    inline uint8_t Pull();

    inline void SetZ(uint8_t value);

    inline void SetN(uint8_t value);

    inline void SetZN(uint8_t value);

    void Compare(uint8_t a, uint8_t b);

    void NMIInterrupt();

    void IRQInterrupt();

    void ADC();

    void AND();

    void ASL();

    void BCC();

    void BCS();

    void BEQ();

    void BIT();

    void BMI();

    void BNE();

    void BPL();

    void BRK();

    void BVC();

    void BVS();

    void CLC();

    void CLD();

    void CLI();

    void CLV();

    void CMP();

    void CPX();

    void CPY();

    void DEC();

    void DEX();

    void DEY();

    void EOR();

    void INC();

    void INX();

    void INY();

    void JMP();

    void JSR();

    void LDA();

    void LDX();

    void LDY();

    void LSR();

    void NOP();

    void ORA();

    void PHA();

    void PHP();

    void PLA();

    void PLP();

    void ROL();

    void ROR();

    void RTI();

    void RTS();

    void SBC();

    void SEC();

    void SED();

    void SEI();

    void STA();

    void STX();

    void STY();

    void TAX();

    void TAY();

    void TSX();

    void TXA();

    void TXS();

    void TYA();

    void INV();

    enum class AddressingMode
    {
        Absolute = 1,
        AbsoluteX,
        AbsoluteY,
        Accumulator,
        Immediate,
        Implied,
        IndexedIndirect,
        Indirect,
        IndirectIndexed,
        Relative,
        ZeroPage,
        ZeroPageX,
        ZeroPageY
    };

    enum class InterruptType
    {
        None, NMI, IRQ
    };

    uint8_t m_A;
    uint8_t m_X;
    uint8_t m_Y;
    uint16_t m_PC;
    uint8_t m_SP;
    uint8_t m_P;

    uint8_t m_Cycles;
    uint8_t m_BranchCycles;
    uint8_t m_OpCode;
    AddressingMode m_Mode;
    uint16_t m_Address;
    InterruptType m_Interrupt;

    Console* m_Console;

    typedef void(CPU::*InstructionFunction)();
    const InstructionFunction m_kExecutionTable[256];
};

END_NES_EMULATOR_NAMESPACE