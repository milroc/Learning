//
//  CalculatorViewController.h
//  Calculator
//
//  Created by Miles McCrocklin on 10/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CalculatorBrain.h"

@interface CalculatorViewController : UIViewController {
    IBOutlet UILabel *display;
    IBOutlet UILabel *units;
    CalculatorBrain *brain;
    BOOL userIsInTheMiddleOfTypingANumber;
    BOOL userPressedDecimal;

}

- (IBAction) digitPressed:(UIButton *)sender;
- (IBAction) decimalPressed:(UIButton *)sender;
- (IBAction) operationPressed:(UIButton *)sender;
- (IBAction) memoryOperationPressed:(UIButton *)sender;
- (IBAction) clear:(UIButton *)sender;
- (IBAction) unitConverterPressed:(UIButton *)sender; //bad naming convention

@end
