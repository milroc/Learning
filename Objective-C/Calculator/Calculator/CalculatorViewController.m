//
//  CalculatorViewController.m
//  Calculator
//
//  Created by Miles McCrocklin on 10/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <math.h>
#import "CalculatorViewController.h"

/*
 For those reading, there are a great deal of fallacies in this code
 e.g: no manipulation of the GUI display if over n digits;
 if (label > sizeof(double)) inf displayed. 
 However this was my "hello world" project into learning Objective-C and I am
 not concerned about the "completeness" of this project. If this irks you, sorry
 I will be forking this and following along CS193P from Stanford Itunes U. 
 These assignments mainly for learning purposes.
 */

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
}

- (IBAction) backspace:(UIButton *)sender
{
    if (1 < [[display text] length]) {
        NSMutableString *str = [NSMutableString stringWithString:
                                [display text]];
        [display setText:[NSString stringWithString:
                          [str substringToIndex:([str length]-1)]]];
        //talk about convoluted
    } else {
        [display setText:@"0"];
    }
    
    
}

- (void) alertView:(UIAlertView *)alert clickedButtonAtIndex:(NSInteger)buttonIndex 
{
    [brain reset];
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
    if (result != result) { //so....odd that this is the conditional for NaN 
        UIAlertView *illegalOperation = [[[UIAlertView alloc] 
                                        initWithTitle:@"Illegal Operation"
                                              message:@"Illegal Operation" 
                                             delegate:self 
                                    cancelButtonTitle:nil 
                                    otherButtonTitles:@"Cancel", @"Ok", nil] 
                                         autorelease];
        [illegalOperation show];
        //show is nonblocking and autorelease means release is not necessary
    } else [display setText:[NSString stringWithFormat:@"%g", result]];
}

- (IBAction) memoryOperationPressed:(UIButton *)sender
{
    //poor design choice...again Controller and Model both need to know
    //about the view....
    if (![@"mc" isEqual:[[sender titleLabel] text]]) {
        if (userIsInTheMiddleOfTypingANumber) {
            [[self brain] setOperand:[[display text] doubleValue]];
            [[self brain] performOperation:@"="]; 
            //ask someone if ^this is too much of a hack
            userIsInTheMiddleOfTypingANumber = NO;
            userPressedDecimal = NO;
        }
        NSString *operation = [[sender titleLabel] text];
        double result = [[self brain] performMemoryOperation:operation];
        [display setText:[NSString stringWithFormat:@"%g", result]];
    } else {
        NSString *operation = [[sender titleLabel] text];
        [[self brain] performMemoryOperation:operation];
        //returning something like this wouldn't cause a memory leak right?
    }

}

- (IBAction) clear:(UIButton *)sender
{
    double result = [[self brain] reset]; 
    //MOpt: why return ^here, just always display 0
    [display setText:[NSString stringWithFormat:@"%g", result]];
    [units setText:@"rad"]; 
    //hmm kind of odd tracking both on both layers...
    userIsInTheMiddleOfTypingANumber = NO;
    userPressedDecimal = NO;
}

//UISwitch
- (IBAction) unitConverterChanged
{
    if (degOrRad.on) {
        [degOrRad setOn:NO animated:YES];
    } else {
        [degOrRad setOn:YES animated:YES];
    }
}

- (IBAction) unitConverterPressed
{
    if ([@"deg" isEqual:[units text]]) {
        [brain setIsDeg:NO];
        [units setText:@"rad"];
    } else if ([@"rad" isEqual:[units text]]) {
        [brain setIsDeg:YES];
        [units setText:@"deg"];
    }
}

@end
