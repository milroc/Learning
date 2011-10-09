//
//  CalculatorViewController.m
//  Calculator
//
//  Created by Miles McCrocklin on 10/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <math.h>
#import "CalculatorViewController.h"

@implementation CalculatorViewController

- (CalculatorBrain *)brain
{
    if (!brain) brain = [[CalculatorBrain alloc] init];
    return brain;
}

- (IBAction) digitPressed:(UIButton *)sender
{
    NSString *digit = [[sender titleLabel] text];
    
    if ([@"π" isEqual:digit]) {
        [display setText:[NSString stringWithFormat: @"%g",M_PI]];
        userIsInTheMiddleOfTypingANumber = YES;
        userPressedDecimal = YES;
    }
    if (userIsInTheMiddleOfTypingANumber) {
        [display setText:[[display text] stringByAppendingString:digit]];
    } else {
        [display setText:digit];
        userIsInTheMiddleOfTypingANumber = YES;
    }
    NSLog(@"Digit %@ pressed", digit);
}

- (IBAction) decimalPressed:(UIButton *)sender
{
    NSString *decimal = [[sender titleLabel] text];
    if (!userPressedDecimal && userIsInTheMiddleOfTypingANumber) {
        [display setText:[[display text] stringByAppendingString:decimal]];
    } else if (!userPressedDecimal) {
        [display setText:@"0."];
        userIsInTheMiddleOfTypingANumber = YES;
    }
    userPressedDecimal = YES;
    NSLog(@"Decimal pressed");
}

- (IBAction) operationPressed:(UIButton *)sender
{
    if (userIsInTheMiddleOfTypingANumber) {
        [[self brain] setOperand:[[display text] doubleValue]];
        userIsInTheMiddleOfTypingANumber = NO;
        userPressedDecimal = NO;
    }
    NSString *operation = [[sender titleLabel] text];
    double result = [[self brain] performOperation:operation];
    [display setText:[NSString stringWithFormat:@"%g", result]];
    NSLog(@"Operation %@ performed", operation);
}

- (IBAction) memoryOperationPressed:(UIButton *)sender
{
    if (userIsInTheMiddleOfTypingANumber) {
        [[self brain] setOperand:[[display text] doubleValue]];
        [[self brain] performOperation:@"="]; //ask someone if this is too much of a hack
        userIsInTheMiddleOfTypingANumber = NO;
        userPressedDecimal = NO;
    }
    NSString *operation = [[sender titleLabel] text];
    double result = [[self brain] performMemoryOperation:operation];
    [display setText:[NSString stringWithFormat:@"%g", result]];
    NSLog(@"Memory Operation: %@ performed", operation);
}

- (IBAction) clear:(UIButton *)sender
{
    double result = [[self brain] reset];
    [display setText:[NSString stringWithFormat:@"%g", result]];
    [units setText:@"rad"]; //hmm kind of odd tracking both on both layers...
    userIsInTheMiddleOfTypingANumber = NO;
    userPressedDecimal = NO;
    NSLog(@"Clear Performed");
}

- (IBAction) unitConverterPressed:(UIButton *) sender
{
    if ([@"deg" isEqual:[[sender titleLabel] text]]) {
        [brain setIsDeg:YES];
        [units setText:@"deg"];
    } else if ([@"rad" isEqual:[[sender titleLabel] text]]) {
        [brain setIsDeg:NO];
        [units setText:@"rad"];
    }
}

@end
