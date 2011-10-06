//
//  CalculatorBrain.h
//  TestCalculator
//
//  Created by Miles McCrocklin on 10/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CalculatorBrain : NSObject
{
    double operand;
}

- (void)setOperand:(double)anOperand;
- (double) performOperation:(NSString *)operation;
@end
