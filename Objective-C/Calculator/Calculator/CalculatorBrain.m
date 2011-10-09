//
//  CalculatorBrain.m
//  Calculator
//
//  Created by Miles McCrocklin on 10/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "CalculatorBrain.h"

/*
 For those reading, there are a great deal of fallacies in this code
 e.g: no manipulation of the GUI display if over n digits;
 if (label > sizeof(double)) inf displayed. 
 However this was my "hello world" project into learning Objective-C and I am
 not concerned about the "completeness" of this project. If this irks you, sorry
 I will be forking this and following along CS193P from Stanford Itunes U. 
 These assignments mainly for learning purposes.
 */

@implementation CalculatorBrain

- (void) setOperand:(double)aDouble
{
    operand = aDouble;
}

- (void) setIsDeg:(BOOL)aBool
{
    isDeg = aBool;
}

- (BOOL) isWaitingOperation
{
    if (waitingOperation == NULL) {
        return YES;
    } else {
        return NO;
    }
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
        else
            operand = NAN;
    }
}

- (double) performOperation:(NSString *)operation
{
    if ([@"sqrt" isEqual:operation]) {
        if (0 < operand) {
            operand = sqrt(operand);
        } else {
            operand = NAN;
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
        NSLog(@"%g",memory);
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
