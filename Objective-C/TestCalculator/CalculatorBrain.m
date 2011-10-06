//
//  CalculatorBrain.m
//  TestCalculator
//
//  Created by Miles McCrocklin on 10/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "CalculatorBrain.h"

@implementation CalculatorBrain

- (void)setOperand:(double)anOperand
{
    operand = anOperand;
}

- (double)performOperation:(NSString *)operation;
{
    [operation sendMessage:argument];
    return aDouble;
}

@end
