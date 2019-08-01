/* Assignment 2
 * Written by Sii Kim Lau
 */


#include <stdio.h>
#include <stdlib.h>

#define BARRIER 999

/* to make sure the origin doesn't have any previous location */
#define NO_PREV -1

#define INITIAL 2

/* number of direction(right, top, left, bottom) */
#define NUM_DIR 4

/* the number of lines we need to print
 * for the grid which included the arrows */
#define PRINT_ROW 3

#define RIGHT 0
#define TOP 1
#define LEFT 2
#define BOT 3
#define NEXT_ROW 1
#define NEXT_COL 1

/* information for grid sizes */
typedef struct {
    int col, row;
    int intersect;
    int possibilities;
} grid_t;

/* information for all the cost in every points */
typedef struct {
    int col;
    char row;
    int cost[NUM_DIR];
} loc_t;

/* store the driver's location and location of the destination(s) */
typedef struct {
    int col;
    char row;
} point_t;

/* store all least cost paths and previous locations(one driver) */
typedef struct {
    int col;
    char row;
    int total_cost;
    int prev_col;
    char prev_row;
} path_driver_t;

/* store all least cost paths and previous locations(multiple drivers) */
typedef struct {
    int col;
    char row;
    int total_cost;
    int prev_col;
    char prev_row;
    int dir[NUM_DIR];
} path_drivers_t;

/* store the path from the destination to the driver's location */
typedef struct {
    int col;
    char row;
    int cost;
} least_route_t;

grid_t grid_size();
void city_grid(grid_t grid, loc_t **grid_info, path_driver_t **path_driver,
               path_drivers_t **path_drivers);
int num_loc(point_t **grid_loc, path_driver_t **path_driver,
            path_drivers_t **path_drivers);
void path_finding(grid_t grid, loc_t **grid_info, path_driver_t **path_driver,
                  path_drivers_t **path_drivers);
int inside_grid(grid_t grid, int row, int col);
void print_path(int num_point, grid_t grid, point_t *grid_loc,
                path_driver_t **path_driver);
void print_closest(grid_t grid, path_drivers_t **path_drivers);
void free_struct(grid_t grid, loc_t **grid_info, path_driver_t **path_driver,
                 path_drivers_t **path_drivers);


int main(int argc, char **argv) {
    int num_point;
    grid_t grid;
    grid = grid_size();
    loc_t **grid_info = (loc_t **)malloc(sizeof(loc_t *)*grid.row);
    path_driver_t **path_driver = (path_driver_t **)malloc
                                  (sizeof(path_driver_t *)*grid.row);
    path_drivers_t **path_drivers = (path_drivers_t **)malloc
                                    (sizeof(path_drivers_t *)*grid.row);
    point_t *grid_loc;
    city_grid(grid, grid_info, path_driver, path_drivers);
    num_point = num_loc(&grid_loc, path_driver, path_drivers);
    path_finding(grid, grid_info, path_driver, path_drivers);
    print_path(num_point, grid, grid_loc, path_driver);
    print_closest(grid, path_drivers);
    free_struct(grid, grid_info, path_driver, path_drivers);
    /* algorithms are fun */
    return 0;
}

/* get the grid size (column x row), number of intersection and
 * total number of possible paths */
grid_t
grid_size() {
    int column, row;
    grid_t grid;
    scanf("%d %d", &column, &row);
    grid.col = column;
    grid.row = row;
    grid.intersect = grid.col * grid.row;
    grid.possibilities = grid.intersect * 4;
    return grid;
}

/* store all the cost of every direction in every points to the structs */
void
city_grid(grid_t grid, loc_t **grid_info, path_driver_t **path_driver,
          path_drivers_t **path_drivers) {
    int column, i, j, k;
    int unusable=0, total_cost=0;
    char row;

    for (i = 0; i < grid.row; i++){
        grid_info[i] = (loc_t *)malloc(sizeof(loc_t)*grid.col);
        path_driver[i] = (path_driver_t *)malloc
                         (sizeof(path_driver_t)*grid.col);
        path_drivers[i] = (path_drivers_t *)malloc
                          (sizeof(path_drivers_t)*grid.col);
    }

    for (i=0; i<grid.row; i++) {
        for (j = 0; j < grid.col; j++) {

            /* store the locations in the correct indexes */
            scanf("%d%c", &column, &row);
            grid_info[i][j].col = column;
            grid_info[i][j].row = row;
            path_driver[i][j].col = column;
            path_driver[i][j].row = row;
            path_drivers[i][j].col = column;
            path_drivers[i][j].row = row;
            for (k = 0; k < NUM_DIR; k++) {
                /* store all the costs for all directions in the location */
                scanf("%d ", &grid_info[i][j].cost[k]);

                /* get the total cost of remaining possibilities */
                if (grid_info[i][j].cost[k] != BARRIER) {
                    total_cost += grid_info[i][j].cost[k];
                }

                /* get the number of unusable direction for the whole grid */
                else {
                    unusable++;
                }
            }

            /* initialise all the locations with cost of 999 */
            path_driver[i][j].total_cost = BARRIER;
            path_drivers[i][j].total_cost = BARRIER;
        }
    }

    printf("S1: grid is %d x %d, and has %d intersections\n", grid.col,
           grid.row, grid.intersect);
    printf("S1: of %d possibilities, %d of them cannot be used\n",
            grid.possibilities, unusable);
    printf("S1: total cost of remaining possibilities is %d seconds\n",
            total_cost);

}

/* get the total number of driver's location and destinations or
 * drivers' locations and return the value */
int
num_loc(point_t **grid_loc, path_driver_t **path_driver,
        path_drivers_t **path_drivers) {
    int column;
    point_t *temp;
    /* assume the total number of locations are two first */
    temp = (point_t *)malloc(sizeof(point_t) * INITIAL);
    *grid_loc = temp;
    char row;
    int max_slot = INITIAL;
    int curr_slot = 0, num_point = 0;
    int start = 1, pos = 0;
    int start_loc_d, last_loc_d;
    char start_loc_l, last_loc_l;

    /* get the driver's location and the destinations or
     * drivers' locations */
    while (scanf("%d%c", &column, &row) != EOF){
        if (start) {
            start_loc_d = column;
            start_loc_l = row;

            /* initialise the driver's location with cost of 0 */
            path_driver[row-'a'][column].total_cost = 0;
            /* no previous location for the driver's location */
            path_driver[row-'a'][column].prev_col = NO_PREV;
            path_driver[row-'a'][column].prev_row = NO_PREV;

            start = 0;
        } else {
            last_loc_d = column;
            last_loc_l = row;
        }

        /* initialise all the drivers' locations with cost of 0 */
        path_drivers[row-'a'][column].total_cost = 0;
        /* no previous location for all the drivers' locations */
        path_drivers[row-'a'][column].prev_col = NO_PREV;
        path_drivers[row-'a'][column].prev_row = NO_PREV;

        temp[pos].col = column;
        temp[pos].row = row;

        /* increase the memory size by 2 times of the current max size */
        if (curr_slot == max_slot-1) {
            max_slot *= 2;
            temp = (point_t*)realloc(temp, sizeof(point_t) * max_slot);
            *grid_loc = temp;
        }
        num_point++;
        curr_slot++;
        pos++;
    }
    printf("S1: %d grid locations supplied, first one is %d%c, last one "
           "is %d%c\n\n", num_point, start_loc_d, start_loc_l,
           last_loc_d, last_loc_l);

    return num_point;
}

/* find the fastest path in all locations from the origin */
void
path_finding(grid_t grid, loc_t **grid_info, path_driver_t **path_driver,
             path_drivers_t **path_drivers) {
    int i, j, k, cost, finish = 0;

    /* direction array to easily identify which neighbour it is approaching */
    int dx[NUM_DIR] = {1, 0, -1, 0};
    int dy[NUM_DIR] = {0, -1, 0, 1};

    /* finish means the whole grid has stored all the least cost paths */
    while (!finish) {
        finish = 1;
        for (i = 0; i < grid.row; i++) {
            for (j = 0; j < grid.col; j++) {
                if (path_driver[i][j].total_cost != BARRIER) {
                    for (k = 0; k < NUM_DIR; k++) {
                        /* if not inside the grid, then ignore it */
                        if (!inside_grid(grid, i + dy[k], j + dx[k])) {
                            continue;
                        }

                        cost = grid_info[i][j].cost[k];
                        if (cost == BARRIER) {
                            continue;
                        }

                        /* get the previous row and column for all locations
                         * with the least cost(apply to both statements as
                         * following) */
                        /* for one driver */

                        /* when the current cost at the location is larger
                         * than the one approaching then replace them with
                         * the smaller one */
                        if (path_driver[i][j].total_cost + cost <
                            path_driver[i + dy[k]][j + dx[k]].total_cost) {

                            finish = 0;
                            path_driver[i + dy[k]][j + dx[k]].total_cost
                                    = path_driver[i][j].total_cost + cost;
                            path_driver[i + dy[k]][j + dx[k]].prev_row = i;
                            path_driver[i + dy[k]][j + dx[k]].prev_col = j;

                        }
                        /* when the current cost at the location is same
                         * as the one approaching then check whether
                         * which one comes from a lower-numbered intersection
                         * then retain that one */
                        else if (path_driver[i][j].total_cost + cost ==
                             path_driver[i + dy[k]][j + dx[k]].total_cost) {

                            if (path_driver[i + dy[k]]
                               [j + dx[k]].prev_col > path_drivers[i][j].col ||
                               (path_driver[i + dy[k]][j + dx[k]].prev_col ==
                                path_driver[i][j].col &&
                                path_driver[i + dy[k]][j + dx[k]].prev_row >
                                path_driver[i][j].row)) {

                                finish = 0;
                                path_driver[i + dy[k]][j + dx[k]].total_cost
                                        = path_driver[i][j].total_cost + cost;
                                path_driver[i + dy[k]][j + dx[k]].prev_row = i;
                                path_driver[i + dy[k]][j + dx[k]].prev_col = j;

                            }
                        }

                        /* for multiple drivers */

                        /* when the current cost at the location is larger
                         * than the one approaching then replace them with
                         * the smaller one */
                        if (path_drivers[i][j].total_cost + cost <
                            path_drivers[i + dy[k]][j + dx[k]].total_cost) {

                            finish = 0;
                            path_drivers[i + dy[k]][j + dx[k]].total_cost
                                    = path_drivers[i][j].total_cost + cost;
                            path_drivers[i + dy[k]][j + dx[k]].prev_row = i;
                            path_drivers[i + dy[k]][j + dx[k]].prev_col = j;

                        }
                        /* when the current cost at the location is same
                         * as the one approaching then check whether
                         * which one comes from a lower-numbered intersection
                         * then retain that one */
                        else if (path_drivers[i][j].total_cost + cost ==
                           path_drivers[i + dy[k]][j + dx[k]].total_cost) {

                            if (path_drivers[i + dy[k]]
                               [j + dx[k]].prev_col > path_drivers[i][j].col ||
                               (path_drivers[i + dy[k]][j + dx[k]].prev_col ==
                                path_drivers[i][j].col &&
                                path_drivers[i + dy[k]][j + dx[k]].prev_row >
                                path_drivers[i][j].row)) {

                                finish = 0;
                                path_drivers[i + dy[k]][j + dx[k]].total_cost
                                        = path_drivers[i][j].total_cost + cost;
                                path_drivers[i + dy[k]][j + dx[k]].prev_row = i;
                                path_drivers[i + dy[k]][j + dx[k]].prev_col = j;

                            }
                        }
                    }
                }
            }
        }
    }
}

/* to determine whether the location is inside the grid or not */
int
inside_grid(grid_t grid, int row, int col) {
    if (row >= 0 && row < grid.row && col >= 0 && col < grid.col) {
        return 1;
    }
    return 0;
}

void
print_path(int num_point, grid_t grid, point_t *grid_loc,
           path_driver_t **path_driver) {
    int i, pos=0, store_pos, curr_row, curr_col, store_row;
    least_route_t store[grid.intersect];

    for (i=1; i<num_point; i++) {
        curr_row = grid_loc[i].row-'a';
        curr_col = grid_loc[i].col;

        /* store all locations before it reaches the origin */
        while (path_driver[curr_row][curr_col].total_cost) {
            store[pos].row = path_driver[curr_row][curr_col].row;
            store[pos].col = path_driver[curr_row][curr_col].col;
            store[pos].cost = path_driver[curr_row][curr_col].total_cost;
            store_row = curr_row;

            /* put the current row and column to the position of previous row
             * and previous column respectively to indicate
             * what is the previous location it came from */
            curr_row = path_driver[curr_row]
                       [curr_col].prev_row;
            curr_col = path_driver[store_row]
                       [curr_col].prev_col;

            pos++;
        }

        /* store the origin locations at the last index in the array */
        if (!path_driver[curr_row][curr_col].total_cost) {
            store[pos].row = path_driver[curr_row][curr_col].row;
            store[pos].col = path_driver[curr_row][curr_col].col;
            store[pos].cost = path_driver[curr_row][curr_col].total_cost;
        }

        store_pos = pos;

        /* print all the locations from the origin
         * to the provided destination(s) */
        for (; pos>=0; pos--) {
            if (pos == store_pos) {
                printf("S2: start at grid %d%c, cost of %d\n",
                       store[pos].col, store[pos].row, store[pos].cost);
            } else {
                printf("S2:       then to %d%c, cost of %d\n",
                       store[pos].col, store[pos].row, store[pos].cost);
            }
        }

        pos = 0;
    }
}

void
print_closest(grid_t grid, path_drivers_t **path_drivers) {
    int i, j, k, total_row = (grid.row - 1) * PRINT_ROW + 1;

    /* direction array to easily identify which neighbour it is approaching */
    int dx[NUM_DIR] = {1, 0, -1, 0};
    int dy[NUM_DIR] = {0, -1, 0, 1};

    printf("\n");
    printf("S3:");

    for (i = 0; i < grid.col; i++) {
        printf("        %d", i);
    }

    printf("\n");
    printf("S3:   +----+");

    for (i = 0; i < grid.col - 1; i++) {
        printf("--------+");
    }

    printf("\n");

    for (i = 0; i < grid.row; i++) {
        for (j = 0; j < grid.col; j++) {
            for (k = 0; k < NUM_DIR; k++) {
                /* if not inside the grid, then ignore it */
                if (!inside_grid(grid, i + dy[k], j + dx[k])) {
                    continue;
                }

                /* previous location of the next point is the location
                 * of the current location */
                if (path_drivers[i][j].row - 'a' == path_drivers[i + dy[k]]
                   [j + dx[k]].prev_row && path_drivers[i][j].col ==
                    path_drivers[i + dy[k]][j + dx[k]].prev_col) {
                    /* to indicate which direction it goes from the
                     * current location */
                    path_drivers[i][j].dir[k] = 1;
                }

                /* means it doesn't go that direction */
                else {
                    path_drivers[i][j].dir[k] = 0;
                }

            }
        }
    }


    for (i = 0; i < grid.row && total_row > 0; i++) {
        /* need to print the row which consists of left and
         * right directions(>>>> or <<<<) only */
        if (total_row % PRINT_ROW == 1) {
            printf("S3: %c |  ", i + 'a');
            for (j = 0; j < grid.col; j++) {
                printf("%3d", path_drivers[i][j].total_cost);
                if (j == grid.col - 1) {
                    printf("\n");
                }

                /* if not inside the grid, then ignore it */
                if (!inside_grid(grid, i, j + NEXT_COL)) {
                    continue;
                }

                /* check whether the current location goes right or not */
                if (path_drivers[i][j].dir[RIGHT]) {
                    printf(" >>>> ");

                }
                /* check whether the current location goes left or not */
                else if (path_drivers[i][j + NEXT_COL].dir[LEFT]) {
                    printf(" <<<< ");
                }

                else {
                    printf("      ");
                }

            }
        }

        /* need to print the row which consists of top or
         * bottom directions(^ or v) only */
        else {
            printf("S3:   |    ");
            i--;
            for (j = 0; j < grid.col; j++) {

                /* check whether the current location goes up or not */
                if (path_drivers[i][j].dir[BOT]) {
                    printf("v        ");
                }

                /* check whether the current location goes up or not */
                else if (path_drivers[i + NEXT_ROW][j].dir[TOP]) {
                    printf("^        ");
                }

                else {
                    printf("         ");
                }

                if (j == grid.col - 1) {
                    printf("\n");
                }

            }
        }
        total_row--;
    }
}

void
free_struct(grid_t grid, loc_t **grid_info, path_driver_t **path_driver,
            path_drivers_t **path_drivers) {
    int i;
    for (i = 0; i < grid.row; i++) {
        free(grid_info[i]);
        free(path_driver[i]);
        free(path_drivers[i]);
    }
    free(grid_info);
    free(path_driver);
    free(path_drivers);
}





