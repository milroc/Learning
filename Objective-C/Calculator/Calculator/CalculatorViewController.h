//
//  CalculatorViewController.h
//  Calculator
//
//  Created by Miles McCrocklin on 10/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
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

@interface CalculatorViewController : UIViewController {
    IBOutlet UILabel *display;
    IBOutlet UILabel *units;
/*  IBOutlet UILabel *operandProgress; no Obj-C learning purpose omitting*/  
/*  IBOutlet UILabel *memDisplay; ^*/
    IBOutlet UISwitch *degOrRad; //bad naming
    CalculatorBrain *brain;
    BOOL userIsInTheMiddleOfTypingANumber;
    BOOL userPressedDecimal;
}

- (IBAction) digitPressed:(UIButton *)sender;
- (IBAction) decimalPressed:(UIButton *)sender;
- (IBAction) backspace:(UIButton *)sender;
- (IBAction) operationPressed:(UIButton *)sender;
- (IBAction) memoryOperationPressed:(UIButton *)sender;
- (IBAction) clear:(UIButton *)sender;
//UISwitch
- (IBAction) unitConverterChanged;
- (IBAction) unitConverterPressed;


@end
