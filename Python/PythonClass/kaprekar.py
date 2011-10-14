def kaprekar_constant(fourDigitNum):
    if(6174 == fourDigitNum):
        return 6174
    st = str(fourDigitNum)
    l = list(st)
    l.sort()
    small = int(''.join(l))
    l.sort(reverse=True)
    big = int(''.join(l))
    return kaprekar_constant(big - small)

print kaprekar_constant(4331)
print kaprekar_constant(3229)
print kaprekar_constant(2134)