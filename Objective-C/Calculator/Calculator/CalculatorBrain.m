//
//  CalculatorBrain.m
//  Calculator
//
//  Created by Miles McCrocklin on 10/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "CalculatorBrain.h"

@implementation CalculatorBrain

- (void) setOperand:(double)aDouble
{
    operand = aDouble;
}

- (void) setIsDeg:(BOOL)aBool
{
    isDeg = aBool;
}

- (void) performWaitingOperation
{
    if ([@"+" isEqual:waitingOperation]) {
        operand = waitingOperand + operand;
    } else if ([@"-" isEqual:waitingOperation]) {
        operand = waitingOperand - operand;
    } else if ([@"*" isEqual:waitingOperation]) {
        operand = waitingOperand * operand;
    } else if ([@"/" isEqual:waitingOperation]) {
        if (operand) 
            operand = waitingOperand / operand;
        //errorCode
    }
}

- (double) performOperation:(NSString *)operation
{
    if ([@"sqrt" isEqual:operation]) {
        if (0 < operand) {
            operand = sqrt(operand);
        } else {
            operand = 0; //figure out a better way to handle this
        }
    } else if ([@"+/-" isEqual:operation]) {
        operand = - operand;
    } else if ([@"sin" isEqual:operation]) {
        if (isDeg) operand = operand*M_PI/180;
        operand = sin(operand);
    } else if ([@"cos" isEqual:operation]) {
        if (isDeg) operand = operand*M_PI/180;
        operand = cos(operand);
    } else if ([@"tan" isEqual:operation]) {
        if (isDeg) operand = operand*M_PI/180;
        operand = tan(operand);
    } else if ([@"1/x" isEqual:operation]) {
        operand = 1 /operand;
    } else {
        [self performWaitingOperation];
        waitingOperation = operation;
        waitingOperand = operand;
    }
    return operand;
}

- (double) performMemoryOperation:(NSString *)operation
{
    if ([@"ms" isEqual:operation]) {
        memory = operand;
    } else if ([@"mr" isEqual:operation]) {
        operand = memory;
    } else if ([@"m+" isEqual:operation]) {
        memory += operand;
        operand = memory;
    } else if ([@"mc" isEqual:operation]) {
        memory = 0;
    }
    return memory;
}

- (double) reset
{
    waitingOperation = NULL;
    operand = 0;
    waitingOperand = 0;
    memory = 0;
    isDeg = NO;
    return operand;
}

@end
