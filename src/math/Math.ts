export const iMath = {


    isPowerOf2(value: number): boolean {
        return (value & (value - 1)) === 0;
    },

    getAlignedSize(size: number, alignment: number = 16): number {
        return Math.ceil(size / alignment) * alignment;
    }

};