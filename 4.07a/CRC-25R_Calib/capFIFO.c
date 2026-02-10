/**
 * \file
 * \details Module for FIFO (First In First Out) circular queue functions.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "capFIFO.h"
#include "crc.h"

/**
 * Create FIFO queue. This function allocates the all memory for the queue.
 * Data is read from the head, head points to the first valid data slot.
 * Data is written to the tail, tail points to the first empty slot after the valid data (bit 0x80000000 is full buffer).
 * \param[out]     out_fifo            Pointer to a Pointer to the FIFO variable.
 * \param[in]      in_array_size       FIFO array size in elements (FIFO queue size is the same).
 * \param[in]      in_element_size     Array element size in bytes.
 * \return         0 = Success (out_fifo IS NOT NULL), Other = Failed (out_fifo is NULL).
 */
int32_t capFIFO_Create(FIFO **out_fifo, int32_t in_array_size, int32_t in_element_size){
  int32_t return_value;

  if(in_array_size >= 2){
    *out_fifo = malloc(sizeof(FIFO));
    if(*out_fifo != NULL){
      (*out_fifo)->array_size = in_array_size;
      (*out_fifo)->element_size = in_element_size;
      (*out_fifo)->head = 0;
      (*out_fifo)->tail = 0;
      (*out_fifo)->array = malloc(in_array_size * in_element_size);
      if(((*out_fifo)->array) != NULL) return_value = FALSE;
      else{
        free(*out_fifo);
        *out_fifo = NULL;
        return_value = TRUE;
      }
    }else{
      *out_fifo = NULL;
      return_value = TRUE;
    }
  }else{
    *out_fifo = NULL;
    return_value = TRUE;
  }
  return return_value;
}

/**
 * Get number of elements in FIFO.
 * \param[in]     in_fifo     Pointer to FIFO variable.
 * \return        Number of elements in the FIFO.
 */
int32_t capFIFO_Used(FIFO *in_fifo){
  int32_t return_value, temp_head, temp_tail;

  if(in_fifo){
    if((in_fifo->tail) & 0x80000000) return_value = (in_fifo->array_size);
    else{
      temp_head = (in_fifo->head);
      temp_tail = (in_fifo->tail);
      return_value = temp_tail - temp_head;
      if(return_value < 0) return_value += (in_fifo->array_size);
    }
  }else return_value = 0;

  return return_value;
}

/**
 * Get the number of free elements in FIFO.
 * \param[in]     in_fifo     Pointer to FIFO variable.
 * \return        Number of free elements in the FIFO.
 */
int32_t capFIFO_Free(FIFO *in_fifo){
  int32_t temp_array_size;

  if(in_fifo){
    temp_array_size = (in_fifo->array_size);
    return temp_array_size - capFIFO_Used(in_fifo);
  }else return 0;
}

/**
 * Is FIFO empty.
 * \param[in]     in_fifo      Pointer to FIFO variable.
 * \return        0 = Not empty, 1 = Empty.
 */
int32_t capFIFO_IsEmpty(FIFO *in_fifo){
  int32_t return_value;

  if(in_fifo){
    if((in_fifo->head) == (in_fifo->tail)) return_value = TRUE;
    else return_value = FALSE;
  }else return_value = TRUE;

  return return_value;
}

/**
 * Is FIFO full.
 * \param[in]      in_fifo     Pointer to FIFO variable.
 * \return         0 = Not full, 1 = Full.
 */
int32_t capFIFO_IsFull(FIFO *in_fifo){
  int32_t return_value;

  if(in_fifo){
    if((in_fifo->tail) & 0x80000000) return_value = TRUE;
    else return_value = FALSE;
  }else return_value = TRUE;

  return return_value;
}

/**
 * Push data into FIFO.
 * \param[out]     out_fifo     Pointer to FIFO variable.
 * \param[in]      in_data      Pointer to data.
 * \param[in]      in_length    Data length in elements.
 * \return         0 = Success (in_length is able to fit into FIFO), Other = Failed (in_length is not able to fit into FIFO).
 */
int32_t capFIFO_Push(FIFO *out_fifo, void *in_data, int32_t in_length){
  int32_t tail_to_end, write_length;
  void *array_ptr;

  if(out_fifo){
    if(in_length){
      if(in_length > 0){
        if(in_length <= capFIFO_Free(out_fifo)){
          array_ptr = (char *)(out_fifo->array) + ((out_fifo->tail) * (out_fifo->element_size));
          tail_to_end = (out_fifo->array_size) - (out_fifo->tail);

          if(in_length <= tail_to_end) write_length = in_length;
          else write_length = tail_to_end;

          memcpy(array_ptr, in_data, write_length * (out_fifo->element_size));

          if(in_length == write_length) (out_fifo->tail) += write_length;
          else{
            array_ptr = (char *)in_data + (write_length * (out_fifo->element_size));
            write_length = in_length - tail_to_end;
            memcpy((out_fifo->array), array_ptr , (write_length * (out_fifo->element_size)));
            (out_fifo->tail) = write_length;
          }
          if((out_fifo->tail) >= (out_fifo->array_size)) (out_fifo->tail) -= (out_fifo->array_size);
          if((out_fifo->head) == (out_fifo->tail)) out_fifo->tail |= 0x80000000;
          return FALSE;
        }else return TRUE;
      }else return TRUE;
    }else return FALSE;
  }else return TRUE;
}

/**
 * Pop data from FIFO.
 * \param[out]     out_fifo       Pointer to FIFO variable.
 * \param[out]     out_data       Pointer to output buffer, NULL=Removal of elements from the queue only.
 * \param[in]      in_length      Number of elements.
 * \param[in]      in_preview     false = Remove elements from queue, true = Queue remains unchanged.
 * \return         false = Success, true = Failed (fifo contains less then in_length).
 */
int32_t capFIFO_Pop(FIFO *out_fifo, void *out_data, int32_t in_length, int32_t in_preview){
  int32_t head_to_end, read_length;
  void *array_ptr;

  if(out_fifo){
    if(in_length){
      if(in_length > 0){
        if(in_length <= capFIFO_Used(out_fifo)){
          array_ptr = (char *)(out_fifo->array) + ((out_fifo->head) * (out_fifo->element_size));
          head_to_end = (out_fifo->array_size) - (out_fifo->head);

          if(in_length <= head_to_end) read_length = in_length;
          else read_length = head_to_end;

          if(out_data != NULL) memcpy(out_data, array_ptr, read_length * (out_fifo->element_size));

          if(in_length == read_length){
            if(!in_preview) (out_fifo->head) += read_length;
          }else{
            if(out_data != NULL) array_ptr = (char *)out_data + (read_length * (out_fifo->element_size));
            read_length = in_length - head_to_end;
            if(out_data != NULL) memcpy(array_ptr, (out_fifo->array), read_length * (out_fifo->element_size));
            if(!in_preview) (out_fifo->head) = read_length;
          }
          if(!in_preview){
            if((out_fifo->head) >= (out_fifo->array_size)) (out_fifo->head) -= (out_fifo->array_size);
            (out_fifo->tail) &= 0x7FFFFFFF;
          }
          return FALSE;
        }else return TRUE;
      }else return TRUE;
    }else return FALSE;
  }else return TRUE;
}

/**
 * View an element from FIFO from the head.
 * \param[in]     in_fifo               Pointer to FIFO variable.
 * \param[in]     in_relative_index     Element index relative to head.
 * \return        Pointer to data element. Returns NULL, if the data element is not in the FIFO queue.
 */
void *capFIFO_Peek(FIFO *in_fifo, int32_t in_relative_index){
  int32_t absolute_index, temp_head, temp_tail;
  void *array_ptr;

  if(in_fifo){
    if((in_fifo->head) != (in_fifo->tail)){
      temp_head = (in_fifo->head);
      temp_tail = (in_fifo->tail) & 0x7FFFFFFF;
      absolute_index = temp_head + in_relative_index;
      while(absolute_index >= (in_fifo->array_size)) absolute_index -= (in_fifo->array_size);
      array_ptr = (char *)(in_fifo->array) + (absolute_index * (in_fifo->element_size));

      if((temp_tail > temp_head) && ((absolute_index < temp_head) || (absolute_index >= temp_tail))) return NULL;
      else if((temp_head > temp_tail) && ((absolute_index < temp_head) && (absolute_index >= temp_tail))) return NULL;
      else return array_ptr;
    }else return NULL;
  }else return NULL;
}

/**
 * View an element from FIFO from the tail.
 * \param[in]     in_fifo               Pointer to FIFO variable.
 * \param[in]     in_relative_index     Element index relative to tail.
 * \return        Pointer to data element. Returns NULL, if the data element is not in the FIFO queue.
 */
void *capFIFO_PeekBackwards(FIFO *in_fifo, int32_t in_relative_index){
  int32_t absolute_index, temp_head, temp_tail;
  void *array_ptr;

  if(in_fifo){
    if((in_fifo->head) != (in_fifo->tail)){
      temp_head = (in_fifo->head);
      temp_tail = (in_fifo->tail) & 0x7FFFFFFF;
      absolute_index = temp_tail - in_relative_index - 1;
      while(absolute_index < 0) absolute_index += (in_fifo->array_size);
      array_ptr = (char *)(in_fifo->array) + (absolute_index * (in_fifo->element_size));

      if((temp_tail > temp_head) && ((absolute_index < temp_head) || (absolute_index >= temp_tail))) return NULL;
      else if((temp_head > temp_tail) && ((absolute_index < temp_head) && (absolute_index >= temp_tail))) return NULL;
      else return array_ptr;
    }else return NULL;
  }else return NULL;
}

/**
 * Find a char value in FIFO.
 * \param[in]     in_fifo     Pointer to FIFO variable.
 * \param[in]     test        Value to find.
 * \return        Index from the buffer head. Returns -1, if the data element is not found in the FIFO queue.
 */
int32_t capFIFO_FindChar(FIFO *in_fifo, char test){
  int32_t i, used, return_value;
  char *ptr;

  return_value = -1;
  used = capFIFO_Used(in_fifo);

  for(i=0; i<used; i++){
    ptr = capFIFO_Peek(in_fifo, i);
    if(*ptr == test){
      return_value = i;
      break;
    }
  }

  return return_value;
}

/**
 * Clears FIFO.
 * \param[out]     out_fifo     Pointer to FIFO variable.
 */
void capFIFO_Clear(FIFO *out_fifo){
  (out_fifo->head) = 0;
  (out_fifo->tail) = 0;
}
