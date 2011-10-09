//
//  CalculatorBrain.h
//  Calculator
//
//  Created by Miles McCrocklin on 10/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CalculatorBrain : NSObject {
    double operand;
    NSString *waitingOperation;
    double waitingOperand;
    double memory;
    BOOL isDeg;
}

- (void) setOperand:(double)aDouble;

- (void) setIsDeg:(BOOL)aBool;

- (double) performOperation:(NSString *)operation;

- (double) performMemoryOperation:(NSString *)operation;

- (double) reset;

@end
