#include "stdafx.h"

#include "sample.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int i_learn_samples::get_inputs_cnt() const
    {
    return inputs_cnt;
    }
//------------------------------------------------------------------------------
int i_learn_samples::get_outputs_cnt() const
    {
    return outputs_cnt;
    }
//------------------------------------------------------------------------------
int i_learn_samples::get_samples_cnt() const
    {
    return samples_cnt;
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
stored_sample::stored_sample():i_learn_samples( 0, 0, 0 )
    {
    inputs_cnt = 0;
    samples_cnt = 0;
    }
//------------------------------------------------------------------------------
float* stored_sample::get_sample_x( int index ) const
    {
    if ( index >= samples_cnt )
        {
        return 0;
        }

    return x[ index ];
    }
//------------------------------------------------------------------------------
float** stored_sample::get_sample_x()
    {
    return x;
    }
//------------------------------------------------------------------------------
float* stored_sample::get_sample_y( int index ) const
    {
    if ( index >= samples_cnt )
        {
        return 0;
        }

    return y[ index ] ;
    }
//------------------------------------------------------------------------------
int stored_sample::load_from_file( char *file_name )
    {
    return load_from_file( file_name, 0 );
    }
//------------------------------------------------------------------------------
int stored_sample::load_from_file( char *file_name, char is_ignore_first_column )
    {      
    FILE *file_stream;
    fopen_s( &file_stream, file_name, "r" );

    fscanf_s( file_stream, "%d", &column_cnt );
    fscanf_s( file_stream, "%d", &row_cnt );    
    fscanf_s( file_stream, "%d", &factor_k );
    if ( is_ignore_first_column ) column_cnt--;

    data = new float* [ row_cnt ];
    for ( int i = 0; i < row_cnt; i++ )
        {
        data[ i ] = new float[ column_cnt ];
        }
    for ( int i = 0; i < row_cnt; i++ )
        {
        if ( is_ignore_first_column )
            {
            fscanf_s( file_stream, "%f" );
            }

        for ( int j = 0; j < column_cnt; j++ )
            {  
            fscanf_s( file_stream, "%f\t", &data[ i ][ j ] );
            }
        }    

#ifdef _DEBUG_SS
    for ( int i = 0; i < row_cnt; i++ )
        {
        printf( "%d : ", i );
        for ( int j = 0; j < column_cnt; j++ )
            {  
            printf( "%8.3f ", data[ i ][ j ] );
            }
        printf( "\n" );
        }
    printf( "\n" );
#endif // _DEBUG_SS 

    fclose( file_stream );

    return 0;
    }
//------------------------------------------------------------------------------
int stored_sample::save_to_file( char *file_name )
    {
    FILE *stream;
    fopen_s( &stream, file_name, "w" );
    fprintf( stream, "%d\n", samples_cnt );
    for ( int i = 0; i < samples_cnt; i++ )
        {
        for ( int j = 0; j < inputs_cnt; j++ )
            {
            fprintf( stream, "%f\t", x[ i ][ j ] );
            }  
        fprintf( stream, "\n" );
        }

    fprintf( stream, "\n" );
    for ( int i = 0; i < samples_cnt; i++ )
        {
        for ( int j = 0; j < outputs_cnt; j++ )
            {
            fprintf( stream, "%f\t", y[ i ][ j ] );
            }  
        fprintf( stream, "\n" );
        }

    fclose( stream );
    return 0;
    }
//------------------------------------------------------------------------------
int stored_sample::create_learn_sample( 
    int inputs_cnt, int learn_samples_count /*= -1*/, 
    int exclude_every_i /*= 1*/, int etalon_column_number /*= 0*/,
    int etalon_columns_cnt /*= 1*/, bool exclude_etalons /*= false */ )
    {
    //#define _DEBUG_SS

    this->inputs_cnt = inputs_cnt;
    this->outputs_cnt = etalon_columns_cnt;

    if ( exclude_etalons == false && inputs_cnt % column_cnt )
        {
#ifdef _DEBUG
        printf( "Windows size [%d] must be multiple of columns count [%d]!\n",
            inputs_cnt, column_cnt );
#endif // DEBUG
        return -1;
        }

    if ( exclude_etalons && inputs_cnt % ( column_cnt - etalon_columns_cnt ) ) 
        {
#ifdef _DEBUG
        printf( "Windows size [%d] must be multiple of columns count [%d]!\n",
            inputs_cnt, column_cnt );
#endif // DEBUG
        return -1;
        }

    int window_column_size = exclude_etalons ?
        inputs_cnt / ( column_cnt - etalon_columns_cnt ) : 
        inputs_cnt / column_cnt;

    float **data_tmp;
    samples_cnt = row_cnt / exclude_every_i;
    if ( row_cnt % exclude_every_i )
        {
        samples_cnt++;
        }
    data_tmp = new float*[ samples_cnt ];
    for ( int i = 0; i < row_cnt; i++ )
        {
        if ( i % exclude_every_i != 0  )
            {
            continue;
            }
        data_tmp[ i / exclude_every_i ] = data[ i ]; 
#ifdef _DEBUG_SS        
        printf( "%d\n", i );
#endif // _DEBUG_SS 
        }
#ifdef _DEBUG_SS        
    printf( "\n" );
#endif

    samples_cnt = samples_cnt - window_column_size;
    if ( samples_cnt <= 0 )
        {
        return 0;
        }

    x = new float*[ samples_cnt ];
    y = new float*[ samples_cnt ];

    //Формирование обучающих выборок.
#ifdef _DEBUG        
    printf( "Creating learning sample (total %d):\n", samples_cnt );
#endif // _DEBUG     

    for ( int i = 0; i < samples_cnt; i++ )
        {

        x[ i ] = new float[ inputs_cnt ];
#ifdef _DEBUG_SS        
        printf( "%2d: [ ", i );
#endif // _DEBUG_SS     

        int finish_column = column_cnt;
        if ( exclude_etalons )
        	{
            finish_column -= etalon_columns_cnt;
        	}

        for ( int k = 0; k < finish_column; k++ )
            {
            for ( int j = 0; j < window_column_size; j++ )
                {                                    
                x[ i ][ k * window_column_size + j ] = 
                    data_tmp[ i + j ][ k ] / factor_k;                
                }
            }    
#ifdef _DEBUG_SS  
        for ( int j = 0; j < window_column_size * column_cnt; j++ )
            {
            printf( "%5.2f ", x[ i ][ j ] );
            }        
#endif // _DEBUG_SS 

        y[ i ] = new float[ etalon_columns_cnt ];
        for ( int j = 0; j < etalon_columns_cnt; j++ )
        	{
            if ( exclude_etalons )
            	{
                y[ i ][ j ] = data_tmp[ i ][ etalon_column_number + j ] / factor_k;
            	}
            else
                {
                y[ i ][ j ] = data_tmp[ i + window_column_size ][ etalon_column_number + j ] / factor_k;
                }            

#ifdef _DEBUG_SS        
            printf( "] -> %5.2f \n", y[ i ][ j ] );
#endif // _DEBUG_SS 
        	}
        }
#ifdef _DEBUG_SS        
    printf( "\n" );
#endif

    if ( learn_samples_count > 0 &&
        learn_samples_count <= samples_cnt )
        {
        samples_cnt = learn_samples_count;
        }

    return 0;    
    }
//------------------------------------------------------------------------------
int stored_sample::get_factor_k() const
    {
    return factor_k;
    }
//------------------------------------------------------------------------------
int stored_sample::get_column_window_size() const
    {
    return inputs_cnt / column_cnt;
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
rt_sample::rt_sample(int samples_cnt, int in_size, int in_var_count,
    int out_size, int out_var_count, int max_var_value) throw (...) :
    i_learn_samples(samples_cnt, in_size, out_size),
    max_var_value(max_var_value),
    in_var_count(in_var_count),
    out_var_count(out_var_count)
{
    if (in_size % in_var_count)
    {
        char msg[200];
        _snprintf(msg, sizeof(msg),
            "rt_sample::rt_sample() - ошибка: число входов (%d) должно "
            "быть кратно числу переменных (%d).",
            in_size, in_var_count);

        throw msg;
    }
    if (out_size % out_var_count)
    {
        char msg[200];
        _snprintf(msg, sizeof(msg),
            "rt_sample::rt_sample() - ошибка: число выходов (%d) должно "
            "быть кратно числу переменных (%d).",
            out_size, out_var_count);

        throw msg;
    }

    x = new float* [samples_cnt];
    for (int i = 0; i < samples_cnt; i++)
    {
        x[i] = new float[in_size];
    }

    y = new float* [samples_cnt];
    for (int i = 0; i < samples_cnt; i++)
    {
        y[i] = new float[out_size];
    }

    for (int i = 0; i < samples_cnt; i++)
    {
        for (int j = 0; j < inputs_cnt; j++)
        {
            x[i][j] = 0;
        }
    }

    for (int i = 0; i < samples_cnt; i++)
    {
        for (int j = 0; j < outputs_cnt; j++)
        {
            y[i][j] = 0;
        }
    }

    fake_image = new float[out_size > in_size ? out_size : in_size];
}
//------------------------------------------------------------------------------
int rt_sample::get_max_var_value() const
{
    return max_var_value;
}
//------------------------------------------------------------------------------
void rt_sample::shift_images()
{
    for (int i = 0; i < samples_cnt - 1; i++)
    {
        for (int j = 0; j < inputs_cnt; j++)
        {
            x[i][j] = x[i + 1][j];
        }
    }

    for (int i = 0; i < samples_cnt - 1; i++)
    {
        for (int j = 0; j < outputs_cnt; j++)
        {
            y[i][j] = y[i + 1][j];
        }
    }
}
//------------------------------------------------------------------------------
void rt_sample::add_new_val_to_in_image(int var_n, float val)
{
    static int var_total_size = inputs_cnt / in_var_count;

    int start_idx = var_n * var_total_size;
    int finish_idx = start_idx + var_total_size - 1;

    if (start_idx >= inputs_cnt)
    {
#ifdef _DEBUG
        printf("rt_sample::add_new_val_to_in_image(...) - "
            "ошибка: выход за диапазон (%d).",
            var_n);
#endif // _DEBUG
    }

    for (int i = start_idx; i < finish_idx; i++)
    {
        x[samples_cnt - 1][i] = x[samples_cnt - 1][i + 1];
    }

    x[samples_cnt - 1][finish_idx] = val /*/ max_var_value*/;
}
//------------------------------------------------------------------------------
void rt_sample::add_new_val_to_out_image(int var_n, float val)
{
    static int var_total_size = outputs_cnt / out_var_count;

    int start_idx = var_n * var_total_size;
    int finish_idx = start_idx + var_total_size - 1;

    if (start_idx >= inputs_cnt)
    {
#ifdef _DEBUG
        printf("rt_sample::add_new_val_to_in_image(...) - "
            "ошибка: выход за диапазон (%d).",
            var_n);
#endif // _DEBUG
    }

    for (int i = start_idx; i < finish_idx; i++)
    {
        y[samples_cnt - 1][i] = y[samples_cnt - 1][i + 1];
    }

    y[samples_cnt - 1][finish_idx] = val/* / max_var_value*/;
}
//------------------------------------------------------------------------------
float* rt_sample::get_sample_x(int index) const
{
    if (index < samples_cnt && index >= 0)
    {
        return x[index];
    }
    else
    {
        char msg[200];
        _snprintf(msg, sizeof(msg),
            "rt_sample::get_sample_x(...) - ошибка: индекс (%d) должен"
            " быть меньше числа образов (%d).",
            index, samples_cnt);

        //throw msg;
    }

    return fake_image;
}
//------------------------------------------------------------------------------
float* rt_sample::get_last_sample_x() const
{
    return x[samples_cnt - 1];
}
//------------------------------------------------------------------------------
float* rt_sample::get_sample_y(int index) const
{
    if (index < samples_cnt && index >= 0)
    {
        return y[index];
    }
    else
    {
        char msg[200];
        _snprintf(msg, sizeof(msg),
            "rt_sample::get_sample_x(...) - ошибка: индекс (%d) должен "
            "быть меньше числа образов (%d).",
            index, samples_cnt);

        //throw msg;
    }

    return fake_image;
}
//------------------------------------------------------------------------------
void  rt_sample::print()
{
    for (int i = 0; i < samples_cnt; i++)
    {
        printf("%3d: [", i);
        for (int j = 0; j < inputs_cnt; j++)
        {
            printf("%.2f ", x[i][j]);
        }
        printf("] -> [");

        for (int j = 0; j < outputs_cnt; j++)
        {
            printf("%.2f ", y[i][j]);
        }
        printf("]\n");
    }
    printf("\n");
}
