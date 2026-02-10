/**;
 * \file
 * \details Public header file for capFIFO module. capFIFO module is for FIFO (First In First Out) circular queue functions.
 */

#ifndef __CAPFIFO_H__
#define __CAPFIFO_H__

#include <stdint.h>

/** Generic struct for defining a circular FIFO queue. */
typedef struct{
  uint32_t array_size;   /**< Size of Array in elements used in the circular FIFO buffer, NOTE: FIFO_Size = Array_Size. */
  uint32_t element_size; /**< Size of an element in array in bytes. */
  uint32_t head;         /**< Array Element Index to FIFO Head, Data is read from the FIFO Head. */
  uint32_t tail;         /**< Array Element Index to FIFO Tail, Data is written to the FIFO Tail. Highest bit (0x80000000) signifies full queue. */
  void *array;           /**< Pointer to FIFO Array. */
} FIFO;

/**
 * Create FIFO queue. This function allocates the all memory for the queue.
 * Data is read from the head, head points to the first valid data slot.
 * Data is written to the tail, tail points to the first empty slot after the valid data (bit 0x80000000 is full buffer).
 * \param[out]     out_fifo            Pointer to a Pointer to the FIFO variable.
 * \param[in]      in_array_size       FIFO array size in elements (FIFO queue size is the same).
 * \param[in]      in_element_size     Array element size in bytes.
 * \return         0 = Success (out_fifo IS NOT NULL), Other = Failed (out_fifo is NULL).
 */
int32_t capFIFO_Create(FIFO **out_fifo, int32_t in_array_size, int32_t in_element_size);

/**
 * Get number of elements in FIFO.
 * \param[in]     in_fifo     Pointer to FIFO variable.
 * \return        Number of elements in the FIFO.
 */
int32_t capFIFO_Used(FIFO *in_fifo);

/**
 * Get the number of free elements in FIFO.
 * \param[in]     in_fifo     Pointer to FIFO variable.
 * \return        Number of free elements in the FIFO.
 */
int32_t capFIFO_Free(FIFO *in_fifo);

/**
 * Is FIFO empty.
 * \param[in]     in_fifo      Pointer to FIFO variable.
 * \return        0 = Not empty, 1 = Empty.
 */
int32_t capFIFO_IsEmpty(FIFO *in_fifo);

/**
 * Is FIFO full.
 * \param[in]      in_fifo     Pointer to FIFO variable.
 * \return         0 = Not full, 1 = Full.
 */
int32_t capFIFO_IsFull(FIFO *in_fifo);

/**
 * Push data into FIFO.
 * \param[out]     out_fifo     Pointer to FIFO variable.
 * \param[in]      in_data      Pointer to data.
 * \param[in]      in_length    Data length in elements.
 * \return         0 = Success (in_length is able to fit into FIFO), Other = Failed (in_length is not able to fit into FIFO).
 */
int32_t capFIFO_Push(FIFO *out_fifo, void *in_data, int32_t in_length);

/**
 * Pop data from FIFO.
 * \param[out]     out_fifo       Pointer to FIFO variable.
 * \param[out]     out_data       Pointer to output buffer, NULL=Removal of elements from the queue only.
 * \param[in]      in_length      Number of elements.
 * \param[in]      in_preview     false = Remove elements from queue, true = Queue remains unchanged.
 * \return         false = Success, true = Failed (fifo contains less then in_length).
 */
int32_t capFIFO_Pop(FIFO *out_fifo, void *out_data, int32_t in_length, int32_t in_preview);

/**
 * View an element from FIFO from the head.
 * \param[in]     in_fifo               Pointer to FIFO variable.
 * \param[in]     in_relative_index     Element index relative to head.
 * \return        Pointer to data element. Returns NULL, if the data element is not in the FIFO queue.
 */
void *capFIFO_Peek(FIFO *in_fifo, int32_t in_relative_index);

/**
 * View an element from FIFO from the tail.
 * \param[in]     in_fifo               Pointer to FIFO variable.
 * \param[in]     in_relative_index     Element index relative to tail.
 * \return        Pointer to data element. Returns NULL, if the data element is not in the FIFO queue.
 */
void *capFIFO_PeekBackwards(FIFO *in_fifo, int32_t in_relative_index);

/**
 * Find a char value in FIFO.
 * \param[in]     in_fifo     Pointer to FIFO variable.
 * \param[in]     test        Value to find.
 * \return        Index from the buffer head. Returns -1, if the data element is not found in the FIFO queue.
 */
int32_t capFIFO_FindChar(FIFO *in_fifo, char test);

/**
 * Clears FIFO.
 * \param[out]     out_fifo     Pointer to FIFO variable.
 */
void capFIFO_Clear(FIFO *out_fifo);

#endif // #ifndef __CAPFIFO_H__
