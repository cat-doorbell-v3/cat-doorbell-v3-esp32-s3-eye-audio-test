


    // !!! This file is generated using emlearn !!!

#include <eml_trees.h>
    

EmlTreesNode meow_nodes[80] = {
  { 7, 5.997368574142456, 1, 6 },
  { 21, -6.982269048690796, 2, 3 },
  { -1, 0, -1, -1 },
  { 32, 5.3759448528289795, 4, 2 },
  { 22, -10.479002952575684, 2, 5 },
  { -1, 1, -1, -1 },
  { 2, -51.934823989868164, 7, 5 },
  { 19, 4.042394995689392, 8, 9 },
  { 25, -3.923321485519409, 2, 5 },
  { 4, -50.755998611450195, 10, 2 },
  { 22, -4.311190247535706, 2, 5 },
  { 23, 0.20581244677305222, 12, 14 },
  { 20, -0.2511342167854309, 13, 2 },
  { 33, -8.749028205871582, 2, 5 },
  { 22, -9.706162452697754, 2, 15 },
  { 34, -0.5802766978740692, 5, 16 },
  { 12, 17.077431678771973, 2, 17 },
  { 14, 6.932565331459045, 2, 5 },
  { 23, -0.12502941489219666, 19, 21 },
  { 8, 3.2887332439422607, 20, 2 },
  { 39, 8.314525604248047, 5, 2 },
  { 0, -215.92599487304688, 2, 22 },
  { 30, 0.8721471130847931, 23, 24 },
  { 2, -69.28726577758789, 2, 5 },
  { 22, 0.49854208528995514, 5, 2 },
  { 7, -1.8176130056381226, 5, 26 },
  { 2, -62.99600791931152, 27, 31 },
  { 1, 128.5245819091797, 28, 30 },
  { 1, 124.36325454711914, 2, 29 },
  { 38, -0.950629286468029, 5, 2 },
  { 13, -12.409455299377441, 5, 2 },
  { 9, 0.16699572652578354, 32, 33 },
  { 38, 3.596340924501419, 5, 2 },
  { 0, -386.2015838623047, 5, 2 },
  { 5, 17.51353168487549, 5, 35 },
  { 11, -9.40528392791748, 36, 39 },
  { 38, -0.11523458361625671, 37, 2 },
  { 39, 7.682239532470703, 38, 2 },
  { 22, -3.269240379333496, 5, 2 },
  { 39, -2.7669652700424194, 5, 2 },
  { 12, 13.612224102020264, 41, 2 },
  { 1, 106.06209182739258, 42, 43 },
  { 9, -5.594367027282715, 5, 2 },
  { 32, 5.3759448528289795, 44, 2 },
  { 16, 4.990853786468506, 5, 2 },
  { 12, 11.96480131149292, 46, 53 },
  { 10, 13.826478004455566, 47, 2 },
  { 37, -5.768081188201904, 2, 48 },
  { 4, -53.691965103149414, 2, 49 },
  { 16, -13.532453060150146, 2, 50 },
  { 17, -8.701143741607666, 2, 51 },
  { 3, 74.85370254516602, 5, 52 },
  { 11, -19.66846466064453, 5, 2 },
  { 2, -50.576866149902344, 2, 5 },
  { 19, 4.179275035858154, 55, 60 },
  { 26, -4.343144059181213, 56, 58 },
  { 25, -8.298383712768555, 5, 57 },
  { 2, -37.86368179321289, 2, 5 },
  { 14, 19.682437419891357, 59, 2 },
  { 18, -13.111900806427002, 2, 5 },
  { 30, 0.16523298621177673, 2, 61 },
  { 37, 1.9724510312080383, 62, 2 },
  { 16, 8.300586700439453, 5, 2 },
  { 0, -327.3218688964844, 5, 64 },
  { 10, 5.263196229934692, 65, 68 },
  { 20, -0.6008707582950592, 66, 2 },
  { 39, 5.169416666030884, 67, 2 },
  { 1, 101.88152313232422, 2, 5 },
  { 36, -1.7417578101158142, 69, 2 },
  { 28, 3.9556002616882324, 5, 70 },
  { 11, 5.837499141693115, 2, 5 },
  { 21, 7.836669206619263, 72, 2 },
  { 19, 5.8406760692596436, 73, 78 },
  { 23, 4.861387252807617, 74, 77 },
  { 16, 4.21752405166626, 75, 2 },
  { 15, -0.23664239794015884, 5, 76 },
  { 4, -19.120893478393555, 5, 2 },
  { 31, -4.3897517919540405, 5, 2 },
  { 17, 1.8815304040908813, 2, 79 },
  { 15, -6.549880027770996, 2, 5 } 
};

int32_t meow_tree_roots[10] = { 0, 11, 18, 25, 34, 40, 45, 54, 63, 71 };

EmlTrees meow = {
        80,
        meow_nodes,	  
        10,
        meow_tree_roots,
    };

static inline int32_t meow_tree_0(const float *features, int32_t features_length) {
          if (features[7] < 5.997369f) {
              if (features[21] < -6.982269f) {
                  return 0;
              } else {
                  if (features[32] < 5.375945f) {
                      if (features[22] < -10.479003f) {
                          return 0;
                      } else {
                          return 1;
                      }
                  } else {
                      return 0;
                  }
              }
          } else {
              if (features[2] < -51.934824f) {
                  if (features[19] < 4.042395f) {
                      if (features[25] < -3.923321f) {
                          return 0;
                      } else {
                          return 1;
                      }
                  } else {
                      if (features[4] < -50.755999f) {
                          if (features[22] < -4.311190f) {
                              return 0;
                          } else {
                              return 1;
                          }
                      } else {
                          return 0;
                      }
                  }
              } else {
                  return 1;
              }
          }
        }
        

static inline int32_t meow_tree_1(const float *features, int32_t features_length) {
          if (features[23] < 0.205812f) {
              if (features[20] < -0.251134f) {
                  if (features[33] < -8.749028f) {
                      return 0;
                  } else {
                      return 1;
                  }
              } else {
                  return 0;
              }
          } else {
              if (features[22] < -9.706162f) {
                  return 0;
              } else {
                  if (features[34] < -0.580277f) {
                      return 1;
                  } else {
                      if (features[12] < 17.077432f) {
                          return 0;
                      } else {
                          if (features[14] < 6.932565f) {
                              return 0;
                          } else {
                              return 1;
                          }
                      }
                  }
              }
          }
        }
        

static inline int32_t meow_tree_2(const float *features, int32_t features_length) {
          if (features[23] < -0.125029f) {
              if (features[8] < 3.288733f) {
                  if (features[39] < 8.314526f) {
                      return 1;
                  } else {
                      return 0;
                  }
              } else {
                  return 0;
              }
          } else {
              if (features[0] < -215.925995f) {
                  return 0;
              } else {
                  if (features[30] < 0.872147f) {
                      if (features[2] < -69.287266f) {
                          return 0;
                      } else {
                          return 1;
                      }
                  } else {
                      if (features[22] < 0.498542f) {
                          return 1;
                      } else {
                          return 0;
                      }
                  }
              }
          }
        }
        

static inline int32_t meow_tree_3(const float *features, int32_t features_length) {
          if (features[7] < -1.817613f) {
              return 1;
          } else {
              if (features[2] < -62.996008f) {
                  if (features[1] < 128.524582f) {
                      if (features[1] < 124.363255f) {
                          return 0;
                      } else {
                          if (features[38] < -0.950629f) {
                              return 1;
                          } else {
                              return 0;
                          }
                      }
                  } else {
                      if (features[13] < -12.409455f) {
                          return 1;
                      } else {
                          return 0;
                      }
                  }
              } else {
                  if (features[9] < 0.166996f) {
                      if (features[38] < 3.596341f) {
                          return 1;
                      } else {
                          return 0;
                      }
                  } else {
                      if (features[0] < -386.201584f) {
                          return 1;
                      } else {
                          return 0;
                      }
                  }
              }
          }
        }
        

static inline int32_t meow_tree_4(const float *features, int32_t features_length) {
          if (features[5] < 17.513532f) {
              return 1;
          } else {
              if (features[11] < -9.405284f) {
                  if (features[38] < -0.115235f) {
                      if (features[39] < 7.682240f) {
                          if (features[22] < -3.269240f) {
                              return 1;
                          } else {
                              return 0;
                          }
                      } else {
                          return 0;
                      }
                  } else {
                      return 0;
                  }
              } else {
                  if (features[39] < -2.766965f) {
                      return 1;
                  } else {
                      return 0;
                  }
              }
          }
        }
        

static inline int32_t meow_tree_5(const float *features, int32_t features_length) {
          if (features[12] < 13.612224f) {
              if (features[1] < 106.062092f) {
                  if (features[9] < -5.594367f) {
                      return 1;
                  } else {
                      return 0;
                  }
              } else {
                  if (features[32] < 5.375945f) {
                      if (features[16] < 4.990854f) {
                          return 1;
                      } else {
                          return 0;
                      }
                  } else {
                      return 0;
                  }
              }
          } else {
              return 0;
          }
        }
        

static inline int32_t meow_tree_6(const float *features, int32_t features_length) {
          if (features[12] < 11.964801f) {
              if (features[10] < 13.826478f) {
                  if (features[37] < -5.768081f) {
                      return 0;
                  } else {
                      if (features[4] < -53.691965f) {
                          return 0;
                      } else {
                          if (features[16] < -13.532453f) {
                              return 0;
                          } else {
                              if (features[17] < -8.701144f) {
                                  return 0;
                              } else {
                                  if (features[3] < 74.853703f) {
                                      return 1;
                                  } else {
                                      if (features[11] < -19.668465f) {
                                          return 1;
                                      } else {
                                          return 0;
                                      }
                                  }
                              }
                          }
                      }
                  }
              } else {
                  return 0;
              }
          } else {
              if (features[2] < -50.576866f) {
                  return 0;
              } else {
                  return 1;
              }
          }
        }
        

static inline int32_t meow_tree_7(const float *features, int32_t features_length) {
          if (features[19] < 4.179275f) {
              if (features[26] < -4.343144f) {
                  if (features[25] < -8.298384f) {
                      return 1;
                  } else {
                      if (features[2] < -37.863682f) {
                          return 0;
                      } else {
                          return 1;
                      }
                  }
              } else {
                  if (features[14] < 19.682437f) {
                      if (features[18] < -13.111901f) {
                          return 0;
                      } else {
                          return 1;
                      }
                  } else {
                      return 0;
                  }
              }
          } else {
              if (features[30] < 0.165233f) {
                  return 0;
              } else {
                  if (features[37] < 1.972451f) {
                      if (features[16] < 8.300587f) {
                          return 1;
                      } else {
                          return 0;
                      }
                  } else {
                      return 0;
                  }
              }
          }
        }
        

static inline int32_t meow_tree_8(const float *features, int32_t features_length) {
          if (features[0] < -327.321869f) {
              return 1;
          } else {
              if (features[10] < 5.263196f) {
                  if (features[20] < -0.600871f) {
                      if (features[39] < 5.169417f) {
                          if (features[1] < 101.881523f) {
                              return 0;
                          } else {
                              return 1;
                          }
                      } else {
                          return 0;
                      }
                  } else {
                      return 0;
                  }
              } else {
                  if (features[36] < -1.741758f) {
                      if (features[28] < 3.955600f) {
                          return 1;
                      } else {
                          if (features[11] < 5.837499f) {
                              return 0;
                          } else {
                              return 1;
                          }
                      }
                  } else {
                      return 0;
                  }
              }
          }
        }
        

static inline int32_t meow_tree_9(const float *features, int32_t features_length) {
          if (features[21] < 7.836669f) {
              if (features[19] < 5.840676f) {
                  if (features[23] < 4.861387f) {
                      if (features[16] < 4.217524f) {
                          if (features[15] < -0.236642f) {
                              return 1;
                          } else {
                              if (features[4] < -19.120893f) {
                                  return 1;
                              } else {
                                  return 0;
                              }
                          }
                      } else {
                          return 0;
                      }
                  } else {
                      if (features[31] < -4.389752f) {
                          return 1;
                      } else {
                          return 0;
                      }
                  }
              } else {
                  if (features[17] < 1.881530f) {
                      return 0;
                  } else {
                      if (features[15] < -6.549880f) {
                          return 0;
                      } else {
                          return 1;
                      }
                  }
              }
          } else {
              return 0;
          }
        }
        

int32_t meow_predict(const float *features, int32_t features_length) {

        int32_t votes[2] = {0,};
        int32_t _class = -1;

    _class = meow_tree_0(features, features_length); votes[_class] += 1;
    _class = meow_tree_1(features, features_length); votes[_class] += 1;
    _class = meow_tree_2(features, features_length); votes[_class] += 1;
    _class = meow_tree_3(features, features_length); votes[_class] += 1;
    _class = meow_tree_4(features, features_length); votes[_class] += 1;
    _class = meow_tree_5(features, features_length); votes[_class] += 1;
    _class = meow_tree_6(features, features_length); votes[_class] += 1;
    _class = meow_tree_7(features, features_length); votes[_class] += 1;
    _class = meow_tree_8(features, features_length); votes[_class] += 1;
    _class = meow_tree_9(features, features_length); votes[_class] += 1;
    
        int32_t most_voted_class = -1;
        int32_t most_voted_votes = 0;
        for (int32_t i=0; i<2; i++) {

            if (votes[i] > most_voted_votes) {
                most_voted_class = i;
                most_voted_votes = votes[i];
            }
        }
        return most_voted_class;
    }
    