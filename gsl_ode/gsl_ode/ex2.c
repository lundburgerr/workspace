#include <stdio.h>
#include <gsl/gsl_vector.h>

int main(){
  size_t n = 10;
  gsl_vector * a = gsl_vector_alloc (n) ; 
  for (int i = 0; i<n; ++i) { 
    gsl_vector_set (a, i,  (double )i+1); 
  }

  FILE *f = fopen("my_file.mat", "w+"); 
  if (f){ 
    gsl_vector_fprintf (f, a, "%f");
  } else {
/* handle error in some intelligent way */
  }
  fclose(f);
  return 0;
}
