//
//  BasicSyntax.m
//  LanguageTricks
//
//  Created by Miles McCrocklin on 10/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "BasicSyntax.h"

/*THIS CODE IS NOT MEANT TO BE RAN */
@implementation BasicSyntax

@synthesize eye; //implements getter/setter

- randomMethod
{
    self.eye; //please use in implementation don't use in setters/getters
    [self randomMethod];
    [super init]; //not good method examples just showcasing self and super
}
@end
