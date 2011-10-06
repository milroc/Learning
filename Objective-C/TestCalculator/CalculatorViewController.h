//
//  CalculatorViewController.h
//  TestCalculator
//
//  Created by Miles McCrocklin on 10/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CalculatorViewController : NSObject
{
    CalculatorBrain * brain;
    IBOutlet UILabel * display;
}

- (IBAction)digitPressed:(UIButton *)sender;
- (IBAction)operationPressed:(UIButton *)sender;

@end
