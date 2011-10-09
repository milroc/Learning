//
//  CalculatorBrain.h
//  Calculator
//
//  Created by Miles McCrocklin on 10/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

/*
 For those reading, there are a great deal of fallacies in this code
 e.g: no manipulation of the GUI display if over n digits;
 if (label > sizeof(double)) inf displayed. 
 However this was my "hello world" project into learning Objective-C and I am
 not concerned about the "completeness" of this project. If this irks you, sorry
 I will be forking this and following along CS193P from Stanford Itunes U. 
 These assignments mainly for learning purposes.
 */

@interface CalculatorBrain : NSObject {
    double operand;
    NSString *waitingOperation;
    double waitingOperand;
    double memory;
    BOOL isDeg;
}

- (void) setOperand:(double)aDouble;

- (void) setIsDeg:(BOOL)aBool;

- (BOOL) isWaitingOperation;

- (double) performOperation:(NSString *)operation;

- (double) performMemoryOperation:(NSString *)operation;

- (double) reset;

@end
