//fastscore.slot.0: in-use
//fastscore.slot.1: in-use

#include "fastscore.h"

#include <vector>
#include <list>

#include <jansson.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 

#include <iostream>
#include <fstream>

//#include <nlohmann/json.hpp>


// modelop.init
void begin()
{
	printf("In Begin()\n");
}


static float dotProduct(std::vector<float> vect_A, std::vector<float> vect_B) { 
    float dot_product = 0; 
    // Loop to calculate dot product 
    for (int i = 0; i < 7; i++) 

        dot_product = dot_product + vect_A[i] * vect_B[i];

    return dot_product; 
} 


static float expit (double x) {
  return 1.0/(1.0+exp(-x));
}


// modelop.score
void action(fastscore_value_t v, int slot, int seqno) {

    //std::ifstream ifs("weights.json");
    //auto weights = json::parse(ifs);  
    
    //std::cout << "weights: " << weights << std::endl;      
    
    //std::vector<float> coefficients = weights["coefficients"];
    //float intercept = weights["intercept"];
    
    std::vector<float> coefficients = {
        0.0245902893,  0.0000560698123,  0.101241351, 
        0.0114262297, -0.0265132668, -0.340226066, -0.173383674}
    float intercept = -0.28894948

    printf("In action got value fmt %d slot %d seqno %d\n", v.fmt, slot, seqno );

    int i;
    
    json_t *a=v.js;
    json_t *b=json_array();
    size_t count=json_array_size(a);
    printf("The input array has %d elements\n", count);

    for (i = 0; i < count; i++)
    {
        json_t *record = json_array_get(a,i);
        json_t *duration_months = json_object_get(record, "duration_months");
        json_t *credit_amount = json_object_get(record, "credit_amount");
        json_t *installment_rate = json_object_get(record, "installment_rate");
        json_t *present_residence_since = json_object_get(record, "present_residence_since");
        json_t *age_years = json_object_get(record, "age_years");
        json_t *number_existing_credits = json_object_get(record, "number_existing_credits");
        json_t *number_people_liable = json_object_get(record, "number_people_liable");

        float *duration_months_value= json_number_value(duration_months);
        float *credit_amount_value= json_number_value(credit_amount);
        float *installment_rate_value= json_number_value(installment_rate);
        float *present_residence_since_value= json_number_value(present_residence_since);
        float *age_years_value= json_number_value(age_years);
        float *number_existing_credits_value= json_number_value(number_existing_credits);
        float *number_people_liable_value= json_number_value(number_people_liable);

        std::vector<float> input_record = {
            duration_months_value, credit_amount_value, installment_rate_value,
            present_residence_since_value, age_years_value, number_existing_credits_value,
            number_people_liable_value};

        float pred_probability = expit(dotProduct(coefficients, input_record) + intercept);

        int prediction;

        if (pred_probability <= 0.5) {
            prediction = 1;
        } else {
            prediction = 2;
        }

	    json_t *response=json_object();

	    json_object_set(response, "prediction", json_real(prediction));
	    json_array_append_new(b, response );
	    printf( "Just appended to array, current length: %d\n", v, json_array_size(b));
    }
    
    printf("Before fastscore_emit\n");
    fastscore_emit((fastscore_value_t) {
        .fmt = FASTSCORE_FMT_JSON,
        .js = b,
    }, 1);
    printf("After fastscore_emit, returning from action()\n");
}


void end1()
{
	printf("End1()\n");
}