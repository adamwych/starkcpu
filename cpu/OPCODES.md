Following is a list of opcodes that a Stark 1-compatible CPU must support.

### 0x00: nop
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|

Does not do anything.

---

### 0x01: setri8 destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 8-bit unsigned integer  | 1       |
| value       | 8-bit signed integer    | 123     |

Copies `value` into given `destination` register.\
Value must be exactly 8 bits.

---

### 0x02: setri16 destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 8-bit unsigned integer  | 1       |
| value       | 16-bit signed integer   | 1234    |

Copies `value` into given `destination` register.\
Value must be exactly 16 bits.

---

### 0x03: setri32 destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 8-bit unsigned integer  | 1       |
| value       | 32-bit signed integer   | 123456  |

Copies `value` into given `destination` register.\
Value must be exactly 32 bits.

---

### 0x04: setraddr destination, [address]
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 8-bit unsigned integer  | 1       |
| address     | 32-bit signed integer   | 0x1000  |

Copies 1 byte from memory at `address` into given `destination` register. 

---

### 0x05: setrr destination, source
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 8-bit unsigned integer  | 0       |
| source      | 8-bit unsigned integer  | 1       |

Copies value from `source` register to `destination` register.

---

### 0x06: setai8 destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 32-bit unsigned integer | 0x1000  |
| value       | 8-bit signed integer    | 1       |

Copies `value` into given `destination` address.\
Address can not be smaller than 0x1000.\
Value must be exactly 8 bits.

---

### 0x07: setai16 destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 32-bit unsigned integer | 0x1000  |
| value       | 16-bit signed integer   | 1       |

Copies `value` into given `destination` address.\
Address can not be smaller than 0x1000.\
Value must be exactly 16 bits.

---

### 0x08: setai32 destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 32-bit unsigned integer | 0x1000  |
| value       | 32-bit signed integer   | 1       |

Copies `value` into given `destination` address.\
Address can not be smaller than 0x1000.\
Value must be exactly 32 bits.

---

### 0x09: setar destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 32-bit unsigned integer | 0x1000  |
| source      | 8-bit unsigned integer  | 0       |

Copies value from `source` register into given `destination` address.

---

### 0x10: setaa destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 32-bit unsigned integer | 0x1000  |
| source      | 32-bit unsigned integer | 0       |

Copies 1 byte located at `source` address into given `destination` address.

---

### 0x11: setrara destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 8-bit unsigned integer  | 0       |
| source      | 8-bit unsigned integer  | 0       |

Copies 1 byte from *address* stored in `source` register into *address* stored in `destination` register.

Example pseudocode:
```
setri32 r0, 0x1000
setri32 r1, 0x1400
setrara r0, r1
```
Assuming value at address `0x1400` is `0xFA`, then `setrara` will copy `0xFA` to address `0x1000`.

---

### 0x12: setraim8 destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 8-bit unsigned integer  | 0       |
| value       | 8-bit signed integer    | 1       |

Copies `value` into *address* stored in `destination` register.
Value must be exactly 8 bits.

---

### 0x13: setraim16 destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 8-bit unsigned integer  | 0       |
| value       | 16-bit signed integer   | 1       |

Copies `value` into *address* stored in `destination` register.
Value must be exactly 16 bits.

---

### 0x14: setraim16 destination, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| destination | 8-bit unsigned integer  | 0       |
| value       | 32-bit signed integer   | 1       |

Copies `value` into *address* stored in `destination` register.
Value must be exactly 32 bits.

---

### 0x20: jmprel address
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| address     | 32-bit unsigned integer | 0x1000  |

Sets instruction pointer to `IP + address`.

---

### 0x21: jmpabs address
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| address     | 32-bit unsigned integer | 0x1000  |

Sets instruction pointer to `address`.

---

### 0x22: jne address
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| address     | 32-bit unsigned integer | 0x1000  |

Sets instruction pointer to `address` if the EQUAL flag is set to 0.

---

### 0x23: inc register
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| register    | 8-bit unsigned integer  | 0       |

Increments value stored by `register` register by 1.

---

### 0x24: dec register
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| register    | 8-bit unsigned integer  | 0       |

Decrements value stored by `register` register by 1.

---

### 0x25: cmpregim32 register, value
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| register    | 8-bit unsigned integer  | 0       |
| value       | 32-bit signed integer   | 0       |

Compares value in `register` register and sets appropriate flags.
Value must be exactly 32 bits.

If values are equal:
- the EQUAL flag is set to 1

If values are not equal:
- the EQUAL flag is set to 0

---

### 0x26: add a, b, destination
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| a           | 8-bit unsigned integer  | 0       |
| b           | 8-bit unsigned integer  | 1       |
| destination | 8-bit unsigned integer  | 2       |

Adds value from register `a` to value in register `b` and puts the result in `destination` register.

---

### 0x27: sub a, b, destination
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| a           | 8-bit unsigned integer  | 0       |
| b           | 8-bit unsigned integer  | 1       |
| destination | 8-bit unsigned integer  | 2       |

Subtracts value from register `a` from value in register `b` and puts the result in `destination` register.

---

### 0x28 mul a, b, destination
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| a           | 8-bit unsigned integer  | 0       |
| b           | 8-bit unsigned integer  | 1       |
| destination | 8-bit unsigned integer  | 2       |

Multiplies value from register `a` by value in register `b` and puts the result in `destination` register.

---

### 0x29: div a, b, destination
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|
| a           | 8-bit unsigned integer  | 0       |
| b           | 8-bit unsigned integer  | 1       |
| destination | 8-bit unsigned integer  | 2       |

Divides value from register `a` by value from register `b` and puts the result in `destination` register.

---

### 0xFF: hlt
| Parameter   | Type                    | Example |
| ----------- |:-----------------------:| -------:|

Immediately stops execution.