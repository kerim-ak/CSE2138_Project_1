// CSE2138 Project#1

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

//This function mimics the strcmp function in the string.h. Compares two strings.
static int ft_strcmp(char *s1, char *s2)
{
	int i;

	i = 0;
	while (s1[i] && s2[i])
	{
		if (s1[i] != s2[i])
			return (1);
		i++;
	}
	if (s1[i] != s2[i])
		return (1);
	return (0);
}

//This function creates an integer array from a string consists of numbers
//and returns that array.
int *char_to_num(char *str, int size)
{
	int *arr = malloc(size * sizeof(int));
    if (!arr)
    {
        printf("Malloc error!\n");
        exit(1);
    }
	int i;

	i = 0;
	while (i <= size && str[i])
	{
		if (str[i] >= '0' && str[i] <= '9')
			arr[i] = str[i] - 48;
		else
			arr[i] = str[i] - 87;
		i++;
	}
	return (arr);
}

//This function changes the places of integers in an array in groups of 2.
//Used for little endian notation.
void reverse_ints(int *arr, int size, int line_size)
{
	int i;
	int j;
	int k;
	int temp_size;
	int temp1;
	int temp2;

	i = 0;
	j = 0;
	k = 0;
	temp1 = 0;
	temp2 = 0;
	temp_size = 0;
	while (j < line_size / size)
	{
		while (k < size / 2)
		{
			temp1 = arr[i];
			temp2 = arr[i + 1];
			arr[i] = arr[temp_size * 2 + size * 2 - i - 2];
			arr[i + 1] = arr[temp_size * 2 + size * 2 - i - 1];
			arr[temp_size * 2 + size * 2 - i - 2] = temp1;
			arr[temp_size * 2 + size * 2 - i - 1] = temp2;
			i += 2;
			k++;
		}
		temp_size += (2 * size);
		i = temp_size;
		j++;
		k = 0;
	}
}

//This function creates an integer array using the bits in the input file.
//Bits represented as integers here. For every hex value, we used an integer.
//E.g.: 1(hex) -> 1(decimal), F(hex) -> 15(decimal)
//Returns the array created from a line in the input file.
static int *convert_int(int size, char endian, int *arr)
{
	int i;
	int j;
	int index;
	int num;
	int *ret;
	
	ret = malloc((12 / size) * sizeof(int));
	if (!ret)
	{
		printf("Malloc error!\n");
		exit(1);
	}
	i = 0;
	j = 0;
	num = 0;
	index = 0;
	if (endian == 'l' && size != 1)
		reverse_ints(arr, size, 12);
	while (j < (12 / size))
	{
		while (i < (size * 2))
		{
			num |= arr[index];
			i++;
			if (i < size * 2)
				num = (num << 4);
			index++;
		}
		if (size == 1 && num >= 128)
			num -= 256;
		if (size == 2 && num >= 32768)
			num -= 65536;
		if (size == 3 && num >= 8388608)
			num -= 16777216;
		ret[j] = num;
		i = 0;
		num = 0;
		j++;
	}
	free(arr);
	return (ret);
}

//This function creates an unsigned integer array using the bits in the input file.
//Bits represented as integers here. For every hex value, we used an integer.
//E.g.: 1(hex) -> 1(decimal), F(hex) -> 15(decimal)
//Returns the array created from a line in the input file.
static unsigned int *convert_uint(int size, char endian, int *arr)
{
	int i;
	int j;
	int index;
	unsigned int num;
	unsigned int *ret;
	
	ret = malloc((12 / size) * sizeof(unsigned int));
	if (!ret)
	{
		printf("Malloc error!\n");
		exit(1);
	}
	i = 0;
	j = 0;
	num = 0;
	index = 0;
	if (endian == 'l' && size != 1)
		reverse_ints(arr, size, 12);
	while (j < (12 / size))
	{
		while (i < (size * 2))
		{
			num |= arr[index];
			i++;
			if (i < size * 2)
				num = (num << 4);
			index++;
		}
		ret[j] = num;
		i = 0;
		num = 0;
		j++;
	}
	free(arr);
	return (ret);
}

//Double number calculator.
double calculate_double(unsigned int sign, unsigned int exponent, unsigned int mantissa, int size)
{
	int bias;
	double mantissa_divider;
	double exp_factor;
	int exp_check;

	if (size == 1)
	{
		bias = 7;
		exp_check = 15;
		mantissa_divider = 8;
	}
	else if (size == 2)
	{
		bias = 31;
		exp_check = 63;
		mantissa_divider = (double)pow(2, 9);
	}
	else if (size == 3)
	{
		bias = 127;
		exp_check = 255;
		mantissa_divider = (double)pow(2, 15);
	}
	else if (size == 4)
	{
		bias = 511;
		exp_check = 1023;
		mantissa_divider = (double)pow(2, 21);
	}
    float mantissa_value;

    if (exponent == 0)
        mantissa_value = (double)mantissa / mantissa_divider;
	else if (exponent == exp_check)
	{
        if (mantissa == 0) 
            return sign ? -INFINITY : INFINITY;
        else
            return NAN;
    }
	else
        mantissa_value = 1.0f + mantissa / mantissa_divider;
	if (exponent == 0)
    	exp_factor = pow(2.0f, (int)exponent - bias + 1);
	else
		exp_factor = pow(2.0f, (int)exponent - bias);
	if (sign)
		return (-1.0f * mantissa_value * exp_factor);
    return (1.0f * mantissa_value * exp_factor);
}

//This function creates a float array using the bits in the input file.
//Bits represented as integers here. For every hex value, we used an integer.
//E.g.: 1(hex) -> 1(decimal), F(hex) -> 15(decimal)
//Returns the array created from a line in the input file.
static double *convert_float(int size, char endian, int *arr)
{
	int i;
	int j;
	int index;
	int sign;
	unsigned int exponent;
	unsigned int mantissa;
	unsigned int temp;
	unsigned int num;
	unsigned int *temp_nums;
	double *ret;
	
	ret = malloc((12 / size) * sizeof(double));
	temp_nums = malloc((12 / size) * sizeof(unsigned int));
	if (!ret || !temp_nums)
	{
		printf("Malloc error!\n");
		exit(1);
	}
	i = 0;
	j = 0;
	num = 0;
	index = 0;
	if (endian == 'l' && size != 1)
		reverse_ints(arr, size, 12);
	while (j < (12 / size))
	{
		while (i < (size * 2))
		{
			num |= arr[index];
			i++;
			if (i < size * 2)
				num = (num << 4);
			index++;
		}
		temp_nums[j] = num;
		i = 0;
		num = 0;
		j++;
	}
	i = 0;
	j = 0;
	index = 0;
	sign = 0;
	exponent = 0;
	mantissa = 0;
	temp = 0;
	while (i < (12 / size))
	{
		sign = 0;
		exponent = 0;
		mantissa = 0;
		temp = temp_nums[i];
		temp >>= (size * 8 - 1);
		if (temp)
			sign = 1;
		temp = temp_nums[i];
		if (size == 1)
		{
			exponent = temp & 120;
			exponent = exponent >> 3;
		}
		else if (size == 2)
		{
			exponent = temp & 32256;
			exponent = exponent >> 9;
		}
		else if (size == 3)
		{
			exponent = temp & 8355840;
			exponent = exponent >> 15;
		}
		else if (size == 4)
		{
			exponent = temp & 2145386496;
			exponent = exponent >> 21;
		}
		temp = temp_nums[i];
		if (size == 1)
            mantissa = (temp & 7);
        else if (size == 2)
            mantissa = (temp & 511);
		else if (size == 3)
		{
			temp = (temp << 9);
			mantissa = (temp >> 9);
			temp = (mantissa & 3);
			mantissa = (mantissa & 32764);
			if (temp)
				mantissa |= 4;
		}
		else
		{
			temp = (temp << 11);
			mantissa = (temp >> 11);
			temp = (mantissa & 255);
			mantissa = (mantissa & 2096896);
			if (temp)
				mantissa |= 256;
		}
		ret[i] = calculate_double(sign, exponent, mantissa, size);
		i++;
	}
	free(arr);
	free(temp_nums);
	return (ret);
}

//Error checker.
static int error_check(int size, char **list)
{
	if (size != 5)
	{
		printf("Incorrect format! The format should be:\n");
		printf("<program_name> <input_file> <endian_type> <data_type> <data_size>\n");
		return (1);
	}
	if (ft_strcmp(list[2], "l") && ft_strcmp(list[2], "b"))
	{
		printf("Wrong endian type! Please select between l and b (little & big)!\n");
		return (1);
	}
	if (ft_strcmp(list[3], "fp") && ft_strcmp(list[3], "i") && ft_strcmp(list[3], "u"))
	{
		printf("Unsupported datatype! Please enter fp, i or u!\n");
		return (1);
	}
	return (0);
}

//Mimic of the atoi function in stdlib.h.
static int ft_atoi(char *str)
{
	int i;
	int sign;
	int result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\v' 
		|| str[i] == '\n' || str[i] == '\f' || str[i] == '\r'
		|| str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign *= -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + str[i] - 48;
		i++;
	}
	return (result * sign);
}

//This function reads a line from a given file pointer and returns it.
//Format: Read 2 chars, skip if space. Read until 24 chars read.
static char *read_from_fd(FILE *input)
{
    int i;
    int j;
    char *str;
    char temp;
    char *ret;

	i = 0;
	j = 0;
	str = malloc(25 * sizeof(char));
	ret = malloc(25 * sizeof(char));
	temp = getc(input);
    while (temp != EOF && temp != '\n')
	{
		if (temp != ' ')
		{
			str[i] = temp;
            i++;
        }
		temp = getc(input);
    }
    str[i] = '\0';
    if (!ret) {
        printf("Malloc error!\n");
        exit(1);
    }
    while (str[j])
	{
        ret[j] = str[j];
        j++;
    }
    ret[j] = '\0';
	free(str);
    return (ret);
}


int main(int argc, char **argv)
{
	FILE *input;
	FILE *output;
	char indian_type;
	char *line;
	int size;
	int *arr;
	unsigned int *u_arr;
	double *d_arr;
	int j;

	if (error_check(argc, argv))
		return (1);
	input = fopen("input.txt", "r");
	if (!input)
	{
		printf("Could not open input file! Check permissions or the existence of the file!\n");
		return (1);
	}
	output = fopen("output.txt", "w+");
	if (!output)
	{
		printf("Could not open output file!\n");
		return (1);
	}
	int i = 0;
	size = ft_atoi(argv[4]);
	if (!ft_strcmp(argv[3], "i"))
	{	
		j = 0;
		while (j < 3)
		{
			line = read_from_fd(input);
			arr = convert_int(size, argv[2][0], char_to_num(line, 24));
			while (i < (12 / size))
			{
				fprintf(output, "%d", arr[i]);
				i++;
				if (i < (12 / size))
					fprintf(output, " ");
			}
			fprintf(output, "\n");
			free(arr);
			i = 0;
			j++;
			free(line);
		}
		
	}
	else if (!ft_strcmp(argv[3], "u"))
	{	
		j = 0;
		while (j < 3)
		{
			line = read_from_fd(input);
			u_arr = convert_uint(size, argv[2][0], char_to_num(line, 24));
			while (i < (12 / size))
			{
				fprintf(output, "%u", u_arr[i]);
				i++;
				if (i < (12 / size))
					fprintf(output, " ");
			}
			fprintf(output, "\n");
			free(u_arr);
			i = 0;
			j++;
			free(line);
		}
		
	}
	else if (!ft_strcmp(argv[3], "fp"))
	{	
		j = 0;
		while (j < 3)
		{
			line = read_from_fd(input);
			d_arr = convert_float(size, argv[2][0], char_to_num(line, 24));
			while (i < (12 / size))
			{
				if (isnan(d_arr[i]))
					fprintf(output, "NaN");
				else if (isinf(d_arr[i]))
				{
					if (d_arr[i] < 0)
						fprintf(output, "-∞");
					else
						fprintf(output, "∞");
				}
				else if (d_arr[i] == 0.0f) 
					fprintf(output, "%d", (int)d_arr[i]);
				else
				{
					if (fabs(d_arr[i]) < 1e-4 || fabs(d_arr[i]) >= 1e6)
						fprintf(output, "%.5e", d_arr[i]);
					else
						fprintf(output, "%.5f", d_arr[i]);
				}
				i++;
				if (i < (12 / size))
					fprintf(output, " ");
			}
			fprintf(output, "\n");
			free(d_arr);
			i = 0;
			j++;
			free(line);
		}
		
	}
	fclose(input);
	fclose(output);
	return (0);
}
