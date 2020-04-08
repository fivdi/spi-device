export type MODE0 = 0;
export type MODE1 = 1;
export type MODE2 = 2;
export type MODE3 = 3;

export type SpiMode = MODE0 | MODE1 | MODE2 | MODE3;

export type SpiOptions = {
    mode?: SpiMode, // 2-bit, MODE0, MODE1, MODE2, or MODE3, default MODE0
    chipSelectHigh?: boolean, // true for active high chip select, default false
    lsbFirst?: boolean, // true for least significant bit first transfer, default false
    threeWire?: boolean, // true for shared MISO/MOSI signals, default false
    loopback?: boolean, // true for loopback mode, default false
    noChipSelect?: boolean, // true for 1 device per bus, no chip select, default false
    ready?: boolean, // true if device pulls low to pause, default false
    bitsPerWord?: number, // 8-bit, device word size, default 8
    maxSpeedHz?: number // 32-bit, device clock frequency in Hertz, default system specific
}

export type SpiMessage = {
    byteLength: number, // 32-bit, the number of bytes to transfer
    sendBuffer?: Buffer, // transmit data
    receiveBuffer?: Buffer, // receive data
    speedHz?: number, // 32-bit, override of the device's clock frequency in Hertz
    microSecondDelay?: number, // 16-bit, delay after the last bit transfer before optionally deselecting the device before the next transfer, default 0
    bitsPerWord?: number, // 8-bit, override of the device's wordsize
    chipSelectChange?: boolean, // true to deselect device before starting the next transfer, default false
}[]

export type CompleteCallback = (err: Error | null | undefined) => void;
export type TransferCallback = (err: Error | null | undefined, message: SpiMessage) => void;

export class SpiDevice {
    transfer(message: SpiMessage, cb: TransferCallback): SpiDevice;
    transferSync(message: SpiMessage): SpiDevice;
    getOptions(cb: CompleteCallback): SpiDevice;
    getOptionsSync(): SpiOptions;
    setOptions(options: SpiOptions, cb: CompleteCallback): SpiDevice;
    setOptionsSync(options: SpiOptions): SpiDevice;
    close(cb: CompleteCallback): void;
    closeSync(): void;
}

export function open(busNumber: number, deviceNumber: number, options: SpiOptions | null | undefined, cb: CompleteCallback): SpiDevice;
export function open(busNumber: number, deviceNumber: number, cb: CompleteCallback): SpiDevice;
export function openSync(busNumber: number, deviceNumber: number, options?: SpiOptions): SpiDevice;
